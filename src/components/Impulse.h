/**
 * Impulse.h
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef IMPULSE_H_
#define IMPULSE_H_

#include "ActivityTimerDistance.h"
#include <common/SimpleCollection.h>
#include "common/Tagged.h"
#include "common/Cycle.h"
#include "common/TimeKeeper.h"
#include <common/Debuggable.h>

#include <list>

namespace cryomesh {

namespace components {

/**
 * Impulse is a mobile information packet to be passed between Nodes
 *
 * Impulses represent information generated by a Node firing
 * They are propagated along a connection
 * Can be modified by the overlying Mesh as they propagate
 */
class Impulse: public common::Tagged, public common::SimpleCollection<double>, public common::Debuggable {
public:

	/**
	 * Constructor
	 *
	 * 		Constructor for Impulse
	 *
	 * @param bool random
	 * 		If true then randomise the impulse on creation
	 */
	Impulse();

	/**
	 * Construct a from a curve with max f(x) and length and
	 * set starting cycle to startCycle, which defaults to the present,
	 * 'now' cycle
	 *
	 *	@param const int max_y
	 * 		Boundary value of curve
	 * 	@param const int length
	 * 		Length of Impulse
	 *  @param  int
	 * 		Delay before starting impulse
	 */
	Impulse(const double max_y, const int length, const int delay = 0);

	/**
	 * Construct a from a curve with max f(x) and length and
	 * set starting cycle to startCycle, and an activity timer
	 *
	 *	@param const int max_y
	 * 		Boundary value of curve
	 * 	@param const int length
	 * 		Length of Impulse
	 *  @param int
	 * 		Delay before starting impulse
	 * 	@param boost::shared_ptr<ActivityTimer> timer
	 * 		The activity timer associated with this
	 */
	Impulse(const double max_y, const int length, const int delay,
			boost::shared_ptr<ActivityTimerDistance>);
	Impulse (const Impulse & obj);
	/**
	 * Destructor
	 *
	 * 		Destructor for Impulse
	 */
	virtual ~Impulse();

	/*
	 * Generate random state
	 *
	 *	@param double
	 *		the (0,1) bias of a positive impulse outcome, 0 negative, 1, positive
	 */
	void randomise(double positive_bias = 0.5);

	/**
	 * Is the Impulse active on current cycle
	 *
	 * @return bool
	 * 		True if active, false otherwise
	 */
	bool isActive() const;

	/**
	 * Is the Impulse active on cycle
	 *
	 * @return bool
	 * 		True if active, false otherwise
	 */
	bool isActive(const common::Cycle & cycle) const;

	/**
	 * Is the Impulse active at some point in cycle range
	 *
	 * @return bool
	 * 		True if active, false otherwise
	 */
	bool isActive(const common::Cycle &startCycle, const common::Cycle &endCycle) const;

	/**
	 * Get activity at cycle
	 *
	 * 		Sum all the Impulses in the collection on specified cycle
	 * 	and return activity
	 *
	 * @param int cycle
	 * 		The cycle to calculate the activity on
	 *
	 * @return double
	 * 		The activity on specified cycle
	 */
	double getActivity(common::Cycle cycle) const;

	/**
	 * Get activity at current cycle
	 *
	 * 		Sum all the Impulses in the collection on the current cycle
	 * 	and return activity
	 *
	 * @return double
	 * 		The activity on specified cycle
	 */
	double getActivity() const;

	/**
	 * Get maximum activity
	 *
	 * 		Find the maximum activity between start and end cycles
	 *
	 * @return double
	 * 		The maximum activity
	 */
	double getActivityMaximum() const;

	/**
	 * Get minimum activity
	 *
	 * 		Find the minimum activity between start and end cycles
	 *
	 * @return double
	 * 		The minimum activity
	 */
	double getActivityMinimum() const;

	/**
	 * Invert the impulse
	 *
	 * @ return Impulse &
	 * 		This object inverted
	 */
	virtual Impulse & invert();

	/**
	 * Get the first active cycle
	 *
	 * @return Cycle
	 * 		The first active cycle
	 */
	common::Cycle getFirstActiveCycle() const;

	/**
	 * Set the first active cycle
	 *
	 * @param const Cycle cycle
	 * 		The first active cycle
	 */
	void setFirstActiveCycle(const common::Cycle cycle);

	/**
	 * Get the last active cycle
	 *
	 * @return Cycle
	 * 		The last active cycle
	 */
	common::Cycle getLastActiveCycle() const;

	/**
	 * Get activities
	 *
	 * @return const std::list<double> &
	 * 		The activities list
	 */
	const std::list<double> & getActivities() const;

	/**
	 * Get activity timer
	 *
	 * @return 	boost::shared_ptr< ActivityTimer > activityTimer;
	 * 		The activity timer
	 */
	const boost::shared_ptr<ActivityTimerDistance> getActivityTimer() const;

	/**
	 * Get mutable activity timer
	 *
	 * @return 	boost::shared_ptr< ActivityTimer > activityTimer;
	 * 		The activity timer
	 */
	boost::shared_ptr<ActivityTimerDistance> getMutableActivityTimer();

	/**
	 * Set activity timer
	 *
	 * @param boost::shared_ptr<ActivityTimer>
	 * 		The activity timer to set
	 */
	void setActivityTimer(boost::shared_ptr<ActivityTimerDistance> timer);

	int getActivityDelay() const ;

	void setActivityDelay(int delay);
	/**
	 * Non-destructive addition operator
	 *
	 * @param const Impulse & obj
	 * 		RHS addition
	 *
	 * @return Impulse
	 * 		New object after addition
	 */
	const Impulse operator+(const Impulse & obj) const;

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const Impulse & obj
	 * 		RHS addition
	 *
	 * @return Impulse &
	 * 		This object after addition and assignment
	 */
	Impulse & operator+=(const Impulse & obj);

	/**
	 * Assignment operator
	 *
	 * @param const Impulse & obj
	 * 		RHS assignment
	 *
	 * @return Impulse &
	 * 		This object after assignment
	 */
	Impulse & operator=(const Impulse & obj);

	/**
	 * Comparator operator
	 *
	 * @param const Impulse & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	bool operator==(const Impulse & obj) const;

	/**
	 * Not comparator operator
	 *
	 * @param const Impulse & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if not equal, false otherwise
	 */
	bool operator!=(const Impulse &obj) const;
	virtual void enableDebug(bool b);

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Impulse & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Impulse & obj);

	/*
	 * Constant for forced trigger activity
	 *
	 * @var double
	 * 		The forced trigger activity
	 */
	static const double FORCED_TRIGGER_ACTIVITY;
	/*
	 * Constant for maximum activity
	 *
	 * @var double
	 * 		The maximum activity
	 */
	static const double MAX_ACTIVITY;

	/*
	 * Constant for minimum activity
	 *
	 * @var double
	 * 		The minimum activity
	 */
	static const double MIN_ACTIVITY;

	/*
	 * Constant for maximum impulse length
	 *
	 * @var int
	 * 		The maximum impulse length
	 */
	static const int MAX_ACTIVITY_LENGTH;

	/*
	 * Constant for minimum impulse length
	 *
	 * @var int
	 * 		The minimum impulse length
	 */
	static const int MIN_ACTIVITY_LENGTH;

	/*
	 * The minimum impulse activity magnitude
	 *
	 * @var int
	 * 		The minimum impulse activity magnitude
	 */
	static const double MIN_ACTIVITY_MAGNITUDE ;

	/*
	 * Constant for minimum impulse delay
	 *
	 * @var int
	 * 		The minimum impulse delay
	 */
	static const int MIN_ACTIVITY_DELAY;

	/*
	 * The maximum impulse activity delay
	 *
	 * @var Cycle
	 * 		The maximum impulse activity magnitude
	 */
	static const int MAX_ACTIVITY_DELAY ;

	/**
	 * Get a 'trigger' impulse, a maximum impulse
	 *
	 * @return boost::shared_ptr<Impulse>
	 * 		The trigger impulse
	 */
	static boost::shared_ptr<Impulse> getTriggerImpulse();

	/**
	 * Get a randomised impulse
	 *
	 *	@param double
	 *		the (0,1) bias of a positive impulse outcome, 0 negative, 1, positive
	 *
	 * @return boost::shared_ptr<Impulse>
	 * The randomised impulse
	 */
	static boost::shared_ptr<Impulse> getRandom(double positive_bias = 0.5);

protected:

	/**
	 * Get the boundary value of activity
	 *
	 * @param bool maximal
	 * 		True if maximal boundary, false if minimal
	 *
	 * @return double
	 * 		The boundary value of activity
	 */
	double getActivityBoundary(bool maximal) const;

private:
	/**
	 * The first cycle that this Impulse has activity
	 *
	 *	@return Cycle
	 * 		Return first active cycle
	 */
	common::Cycle firstActiveCycle;

	/**
	 * The lase cycle that this Impulse has activity
	 *
	 *	@return Cycle
	 * 		Return last active cycle
	 */
	common::Cycle lastActiveCycle;

	/**
	 * Delay on the impulse
	 *
	 * @var int
	 */
	int activityDelay;

	/**
	 * The timer component of the impulse
	 *
	 * @var boost::shared_ptr< ActivityTimer >
	 */
	boost::shared_ptr<ActivityTimerDistance> activityTimer;
};

}

}

#endif /* IMPULSE_H_ */
