/*
 * ActivityTimer.h
 *
 *  Created on: 14 Feb 2011
 *      Author: niall
 */

#ifndef ACTIVITYTIMER_H_
#define ACTIVITYTIMER_H_

#include <ostream>

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

	virtual void reset() = 0;

	/**
		 * To stream operator
		 *
		 *	@param std::ostream & os
		 *		The output stream
		 *	@param const ActivityTimer & obj
		 *		The object to stream
		 *
		 *	@return std::ostream &
		 *		The output stream
		 */
		friend std::ostream& operator<<(std::ostream & os, const ActivityTimer & obj){
			os<<obj.print(os);
			return os;
		}

protected:
	virtual std::ostream & print(std::ostream & os) const =0;
};

}

}

#endif /* ACTIVITYTIMER_H_ */
