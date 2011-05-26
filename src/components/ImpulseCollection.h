/**
 * ImpulseCollection.h
 *
 *  Created on: 20 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 *
 *      A collection of Impulses that allows for Impulses to be held,
 *      'moved forward' in time, and summated in some way
 *
 */

#ifndef IMPULSECOLLECTION_H_
#define IMPULSECOLLECTION_H_

#include "Impulse.h"
#include "common/Cycle.h"

#include "common/KeyMappedCollection.h"
#include "dataobjects/DataObjectController.h"

#include <boost/uuid/uuid.hpp>
#include "common/Debuggable.h"
#include <map>

namespace cryomesh {

namespace components {

/**
 * ImpulseCollection represents a collection of Impulse objects
 *
 *      A collection of Impulses that allows for Impulses to be held,
 *      'moved forward' in time, and summated in some way
 */
class ImpulseCollection: public common::Debuggable, public common::KeyMappedCollection<boost::uuids::uuid, Impulse>, public dataobjects::DataObjectController <unsigned long int, double>{
public:

	enum Comparison {
		GreaterThan, LessThan, EqualTo, NotEqualTo, LessThanOrEqualTo, GreaterThanOrEqualTo
	};
	/**
	 * Contructor for ImpulseCollection
	 *
	 *   Contruct using default Mesh
	 */
	ImpulseCollection();

	/**
	 * Destructor for ImpulseCollection
	 *
	 *   Destructor
	 */
	virtual ~ImpulseCollection();

	/**
	 * Get activity at cycle
	 *
	 * 		Sum all the Impulses in the collection on specified cycle
	 * 	and return activity
	 *
	 * @param Cycle cycle
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
	 * Clear collection up to present cycle
	 *
	 * 		Update the collection to present cycle (non-inclusive) by dropping all impulses that
	 * are 'in the past' relative to that cycle. Interval is [0,present_cycle)
	 *
	 */
	void clearImpulses();

	/**
	 * Clear collection up to specified cycle
	 *
	 * 		Update the collection to specified cycle (non-inclusive) by dropping all impulses that
	 * are 'in the past' relative to that cycle. Interval is [0,cycle)
	 *
	 * @param common::Cycle cycle
	 * 		The cycle that is the cutoff point for the collection
	 */
	void clearImpulses(common::Cycle cycle);

	/**
	 *	Clear the Impulses that start on or after cycle start parameter and finish before
	 *	cycle end parameter.  Interval is [cycle_start,cycle_end)
	 *
	 *	@param Cycle cycleStart
	 *		Cycle parameter that marks the start of the cleared area
	 *	 @param Cycle cycleEnd
	 *		Cycle parameter that marks the end of the cleared area (non-inclusive)
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The deleted collection of impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearImpulses(common::Cycle cycleStart,
			common::Cycle cycleEnd);

	/**
	 * Clear cycles that are active on this cycle
	 *
	 * 		Update the collection to by dropping all impulses that
	 * are active on this cycle
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The  collection of deleted impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActiveImpulses();

	/**
	 * Clear cycles that are active on  cycle
	 *
	 * 		Update the collection to by dropping all impulses that
	 * are active on  cycle
	 *
	 * @param common::Cycle cycle
	 * 		The cycle to drop inclusive impulses from
	 *
	 * 	 @return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The  collection of deleted impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActiveImpulses(common::Cycle cycle);

	/**
	 * Clear cycles that are active on cycle range.  Interval is [cycle_start,cycle_end)
	 *
	 * 		Update the collection to by dropping all impulses that
	 * are active on cycle range
	 *
	 * @param common::Cycle cycleStart
	 * 		The start cycle to drop inclusive impulses from
	 * @param common::Cycle cycleEnd
	 * 		The end cycle to drop inclusive impulses from excluded
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The  collection of deleted impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActiveImpulses(common::Cycle cycleStart,
			common::Cycle cycleEnd);

	/**
	 *	Clear the Impulses that have activities less than parameter
	 *
	 *	@param double activity
	 *		The minimum activity impulses must have to avoid deleteion
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The deleted collection of impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActivitiesByMinimum(double activity);

	/**
	 *	Clear the Impulses that have activities greater than parameter
	 *
	 *	@param double activity
	 *		The maximum activity impulses must have to avoid deleteion
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The deleted collection of impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActivitiesByMaximum(double activity);

	/**
	 * Decrement the activity timers of all impulses
	 */
	void decrementActivityTimers();

	/**
	 * Get impulse list by activity timer value
	 *
	 * @param double value
	 * 		activity timer value
	 *	@param Comparison comp
	 * 		What comparison to make with the value
	 *
	 *	@return std::list<boost::shared_ptr< Impulse> >
	 *		The list of impulses that meet the comparison
	 */
	std::list<boost::shared_ptr<Impulse> > getByActivityTimerValue(double value, Comparison comp);

	/**
	 * remove impulses  by activity timer value
	 *
	 * @param double value
	 * 		activity timer value
	 *	@param Comparison comp
	 * 		What comparison to make with the value
	 *
	 *	@return std::list<boost::shared_ptr< Impulse> >
	 *		The  that meet the comparison and were removed
	 */
	std::list<boost::shared_ptr<Impulse> > removeByActivityTimerValue(double value=0, Comparison comp=LessThanOrEqualTo);

	/**
	 * Inherited from DataObjectController. Overriden to force refresh update on call
	 */
	virtual void refreshDataObject();

	virtual void enableDebug(bool b);
		//operators

	/**
	 * Assignment operator
	 *
	 * @param const ImpulseCollection & obj
	 * 		RHS assignment
	 *
	 * @return ImpulseCollection &
	 * 		This object after assignment
	 */
	ImpulseCollection & operator=(const ImpulseCollection & obj);

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const ImpulseCollection & obj
	 * 		RHS addition
	 *
	 * @return ImpulseCollection &
	 * 		This object after addition and assignment
	 */
	ImpulseCollection & operator+=(const ImpulseCollection & obj);

	/**
	 * Non-destructive addition operator
	 *
	 * @param const ImpulseCollection & obj
	 * 		RHS addition
	 *
	 * @return ImpulseCollection
	 * 		New object after addition
	 */
	const ImpulseCollection operator+(const ImpulseCollection & obj) const;

	/**
	 * Comparator operator
	 *
	 * @param const ImpulseCollection & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	bool operator==(const ImpulseCollection & obj) const;

	/**
	 * Not comparator operator
	 *
	 * @param const ImpulseCollection & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if not equal, false otherwise
	 */
	bool operator!=(const ImpulseCollection &obj) const;

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const ImpulseCollection & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const ImpulseCollection & obj);

	// GETTERS/SETTERS

	/**
	 * Get the associated Mesh
	 *
	 * @return Mesh
	 *
	 const boost::shared_ptr<Mesh>
	 getMesh() const;
	 */

protected:
	/**
	 *	Clear the Impulses that have activities greater or less than parameter
	 *
	 *	@param double activity
	 *		The maximum or minimum activity impulses must have to avoid deleteion
	 *@param bool
	 * 		True is first parameter is maximum allowed value, false if its the minimum
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The deleted collection of impulses
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > clearActivitiesByValue(double activity, bool greater);
private:

};

}

}

#endif /* IMPULSECOLLECTION_H_ */
