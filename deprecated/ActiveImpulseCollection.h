/**
 * ActiveImpulseCollection.h
 *
 *  Created on: 9 Feb 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef ACTIVEIMPULSECOLLECTION_H_
#define ACTIVEIMPULSECOLLECTION_H_

#include "ImpulseCollection.h"
#include "ActivityTimer.h"

#include <boost/uuid/uuid.hpp>
/**
 *  A templated ImpulseCollection in which the elements are also considered active in
 *  some way. For example, each impulse may be counting down a length or
 *  time that represents a journey along a connection. The template parameter is some
 *  form of class that can be updated and triggered on some criteria
 */
namespace cryomesh {

namespace components {

class ActiveImpulseCollection {
public:
	/**
	 * Constructor
	 */
	ActiveImpulseCollection();

	/**
	 * Destructor
	 */
	virtual ~ActiveImpulseCollection();

	/**
	 * Update all the elements by one step
	 */
	void update();

	/**
	 * Update all the elements to step represented by ActivityTimer
	 *
	 * @param ActivityTimer& step
	 */
	void update(const ActivityTimer & step);

	/**
	 * Get all the current step activated impulses
	 *
	 * @return ImpulseCollection
	 * 		The active impulse collections
	 */
	ImpulseCollection getActivatedImpulsesOnCurrentStep() const;

	/**
	 * Get all the activated impulses on a step
	 *
	 * @param const ActivityTimer & step
	 * 		The step on which impulses are active
	 *
	 * @return ImpulseCollection
	 * 		The active impulse collections
	 */
	ImpulseCollection getActivatedImpulsesOnStep(const ActivityTimer & step) const;

	/**
	 * Get all the impulses active on or before a step
	 *
	 * @param const ActivityTimer & step
	 * 		The step on or beore which impulses are active
	 *
	 * @return ImpulseCollection
	 * 		The active impulse collections
	 */
	ImpulseCollection getActivatedImpulsesBeforeStep(const ActivityTimer & step) const;

	/**
	 * Get all the impulses active within step range.
	 * [start, end)
	 *
	 * @param const ActivityTimer & stepStart
	 * 		The step start range
	 * @param const ActivityTimer & stepEnd
	 * 		The step end range, non-inclusive
	 *
	 * @return ImpulseCollection
	 * 		The active impulse collections
	 */
	ImpulseCollection
			getActivatedImpulsesOnStepRange(const ActivityTimer & stepStart, const ActivityTimer & stepEnd) const;

	/**
	 * Get all Impulses
	 *
	 * @return ImpulseCollection &
	 * 		The underlying impulse collection
	 */
	const ImpulseCollection & getImpulses() const;

	/**
	 * Get all mutable Impulses
	 *
	 * @return ImpulseCollection &
	 * 		The underlying impulse collection
	 */
	ImpulseCollection & getImpulses();

private:

	common::StepMappedCollection<boost::uuids::uuid, components::ActivityTimer> stepMappedImpulses;
	/**
	 * The collection of all impulses
	 *
	 * 	@var ImpulseCollection
	 */
	ImpulseCollection impulses;
};

}

}

#endif /* ACTIVEIMPULSECOLLECTION_H_ */
