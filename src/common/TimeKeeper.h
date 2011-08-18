/*
 * TimeKeeper.h
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 */

#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include "common/Cycle.h"

#include <boost/shared_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>

#include <time.h>


namespace cryomesh {

namespace common {

/**
 * TimeKeeper is a class keep track of the cycle state and timing
 * 
 * 		TimeKeeper manages the 'tick' cycle and provides the means by which
 *	classes throughout the system can keep track of the cycle
 */
class TimeKeeper {

public:

#ifdef ENABLE_SERIALISATION
	friend class boost::serialization::access;
	/**
	 * Implementation for boost serialisation interface
	 *
	 * @param Archive ar
	 * 		Archive to serialise to
	 * @param unsigned int version
	 * 		Version of boost serialisation interface
	 */
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & cycleInt;
		ar & timekeeper;
		ar & start_time;
		ar & last_timing;
		ar & this_timing;
	}
#endif // ENABLE_SERIALISATION

	static  TimeKeeper & getTimeKeeper();

	/**
	 * Destructor
	 *
	 * 		Destructor for TimeKeeper
	 */

	/**
	 * Reset the timekeeper
	 */
	void reset();
	virtual ~TimeKeeper();

	/**
	 * Equality test override for object
	 *
	 * 	@param TimeKeeper obj
	 * 		Object to compare this with
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	bool operator==(const TimeKeeper &);

	/**
	 * Move the timing on one cycle
	 */
	void update();

	/**
	 * Get the current cycle we're on as an Cycle
	 *
	 * @return Cycle
	 * 		The cycle we're currently on
	 */
	Cycle  getCycle() const;

	/**
	 * Get the time we started cycling
	 *
	 * @return time_t
	 * 		The start time
	 */
	std::time_t   getStartTime()const;

	/**
	 * Difference between the last time and now
	 *
	 * @return double
	 * 		The difference between the clock now and the last clock
	 */
	double  getTiming()const;

	/**
	 * Get the Timer
	 *
	 * @return boost::Timer
	 */
	const   boost::timer & getTimer() const;
protected:

	/**
	 * Constructor
	 *
	 * 		Constructor for TimeKeeper. Inaccessible to force singleton class
	 */
	TimeKeeper();

	/**
	 * Copy Constructor
	 *
	 * 		Overridden Copy Contructor for TimeKeeper. Inaccessible to force singleton class
	 *
	 * @param TimeKeeper
	 * 		Object to Copy Construct from
	 */
	TimeKeeper(const TimeKeeper &);

	/**
	 * Assignment Operator
	 *
	 * 		Overridden Assignment Operator for TimeKeeper. Inaccessible to force singleton class
	 *
	 *  @param TimeKeeper
	 * 		Object to Assign this to
	 */
	TimeKeeper & operator=(const TimeKeeper &);

private:

	/**
	 * The single instance of a TimeKeeper
	 *
	 * @var boost::shared_ptr<TimeKeeper>
	 */
	static boost::shared_ptr<TimeKeeper> timekeeper;


	/**
	 * The current cycle
	 *
	 * @var Cycle
	 */
	Cycle cycle;

	/**
	 * Time we were initialised (program start)
	 *
	 * @var std::time_t
	 */
	std::time_t start_time;

	/**
	 * Timer
	 *
	 * @var boost::timer
	 */
	boost::timer timer;

	/**
	 * The current cycle's start time, elapsed since timer start
	 *
	 * @var double
	 */
	double this_timing;

	/**
	 * The previous ticks start time, elapsed since timer start
	 *
	 * @var double
	 */
	double last_timing;

};

}

}

#endif /* TIMEKEEPER_H_ */
