/*
 * ActivityTimer.h
 *
 *  Created on: 14 Feb 2011
 *      Author: niall
 */

#ifndef ACTIVITYTIMER_H_
#define ACTIVITYTIMER_H_

namespace cryomesh {

namespace components {

/**
 * Simple interface class for activity timers
 */
class ActivityTimer {
public:
	/**
	 * Default constructor
	 */
	ActivityTimer() {
	}

	/**
	 * Default destructor
	 */
	virtual ~ActivityTimer() {
	}
};

}

}

#endif /* ACTIVITYTIMER_H_ */
