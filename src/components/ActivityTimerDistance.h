/*
 * ActivityTimerDistance.h
 *
 *  Created on: 14 Feb 2011
 *      Author: niall
 */

#ifndef ACTIVITYTIMERDISTANCE_H_
#define ACTIVITYTIMERDISTANCE_H_

#include "ActivityTimer.h"
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace components {

class ActivityTimerDistance: public ActivityTimer {
public:

	/**
	 * Constructor
	 */
	ActivityTimerDistance();

	/**
	 * Construct from an initial distance, and a rate of decrement
	 */
	ActivityTimerDistance(double dist, double dec);
	virtual ~ActivityTimerDistance();

	/**
	 * Assignment operator
	 *
	 * @param const ActivityTimerDistance & obj
	 * 		RHS assignment
	 *
	 * @return ActivityTimerDistance &
	 * 		This object after assignment
	 */
	ActivityTimerDistance & operator=(const ActivityTimerDistance & obj);

	/**
	 * Less than operator
	 *
	 * @param const ActivityTimerDistance & obj
	 * 		RHS
	 *
	 * @return bool
	 * 		True if < than obj, false otherwise
	 */
	bool operator<(const ActivityTimerDistance & obj) const;

	/**
	 * Greater than operator
	 *
	 * @param const ActivityTimerDistance & obj
	 * 		RHS
	 *
	 * @return bool
	 * 		True if > than obj, false otherwise
	 */
	bool operator>(const ActivityTimerDistance & obj) const;

	/**
	 * Prefix increment operator
	 *
	 * @return ActivityTimerDistance &
	 * 		Return this
	 */
	ActivityTimerDistance & operator--();

	/**
	 * Postfix decrement operator
	 *
	 * @return ActivityTimerDistance &
	 * 		Return this
	 */
	ActivityTimerDistance operator--(int);

	/**
	 * Non-destructive addition operator
	 *
	 * @param const ActivityTimerDistance & obj
	 * 		RHS addition
	 *
	 * @return ActivityTimerDistance
	 * 		New object after addition
	 */
	const ActivityTimerDistance operator+(const ActivityTimerDistance & obj) const;

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const ActivityTimerDistance & obj
	 * 		RHS addition
	 *
	 * @return ActivityTimerDistance &
	 * 		This object after addition and assignment
	 */
	ActivityTimerDistance & operator+=(const ActivityTimerDistance & obj);

	/**
	 * Get the delay
	 *
	 * @return double
	 * 		The distance delay
	 */
	double getDelay()const;

	/**
	 * Get the starting delay
	 *
	 * @return double
	 * 		The starting distance delay
	 */
	double getStartingDelay()const;


	/**
	 * Get the decrement
	 *
	 * @return double
	 * 		The decrement of the timer
	 */
	double getDecrement()const;

	/**
	 * Reset the countdown
	 */
	void reset();

	/**
	 * Get a random object
	 *
	 * @return boost::shared_ptr<ActivityTimerDistance>
	 * 		The random object
	 */
	static boost::shared_ptr<ActivityTimerDistance> getRandom();

	/**
	 * The Min decrement fraction of the distance
	 *
	 * @var double
	 */
	static const double MIN_DECREMENT_FRACTION;

	/**
	 * The Max decrement fraction of the distance
	 *
	 * @var double
	 */
	static const double MAX_DECREMENT_FRACTION;

	/**
	 * The Min distance
	 *
	 * @var double
	 */
	static const double MIN_DISTANCE;

	/**
	 * The Man distance
	 *
	 * @var double
	 */
	static const double MAX_DISTANCE;

protected:
	virtual std::ostream & print(std::ostream & os)const;

private:
	/**
	 * Variable representing the abstract starting 'distance' until the end of the timer
	 *
	 * @var double
	 */
	double distance;

	/**
		 * Variable representing the abstract 'distance' until the end of the timer
		 *
		 * @var double
		 */
		double distance_remaining;


	/**
	 * The decrement to the distance each cycle
	 *
	 * @var double
	 */
	double decrement;
};

}

}

#endif /* ACTIVITYTIMERDISTANCE_H_ */
