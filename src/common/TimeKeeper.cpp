/*
 * TimeKeeper.cpp
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 */

#include "TimeKeeper.h"
#include <iostream>
#include <ctime>
#include <time.h>

namespace cryomesh {

namespace common {

boost::shared_ptr<TimeKeeper> TimeKeeper::timekeeper;

TimeKeeper & TimeKeeper::getTimeKeeper() {
	if (timekeeper == 0) {
		timekeeper = boost::shared_ptr<TimeKeeper>(new TimeKeeper);
	}
	return *timekeeper;
}

void TimeKeeper::reset() {
	this->cycle = Cycle(0);
	this_timing =0;
	last_timing = 0;
	std::time(&start_time);
	timer.restart();
}

TimeKeeper::TimeKeeper() : cycle (Cycle(0)), start_time(), timer(),this_timing (0), last_timing ( 0){
	this->reset();
}
TimeKeeper::~TimeKeeper() {
}

bool TimeKeeper::operator==(const TimeKeeper & obj) {
	bool match = true;

	match = match && (this->cycle == obj.cycle);
	if (match == false) {
		std::cout << "TimeKeeper::operator ==: cycle mismatch " << std::endl;
		return false;
	}
	match = match && (this->start_time == obj.start_time);
	if (match == false) {
		std::cout << "TimeKeeper::operator ==: start_time mismatch " << std::endl;
		return false;
	}
	match = match && (this->last_timing == obj.last_timing);
	if (match == false) {
		std::cout << "TimeKeeper::operator ==: last_timing mismatch " << std::endl;
		return false;
	}
	match = match && (this->this_timing == obj.this_timing);
	if (match == false) {
		std::cout << "TimeKeeper::operator ==: this_timing mismatch " << std::endl;
		return false;
	}

	return true;
}
void TimeKeeper::update() {
	this->cycle += 1;

	this->last_timing = this->this_timing;
	this_timing = timer.elapsed();
	//std::cout << "TimeKeeper::update: " << "this_timing:" << this->last_timing << " last_timing:" << this->this_timing
	//		<< std::endl;

}

Cycle TimeKeeper::getCycle() const {
	return this->cycle;
}

double TimeKeeper::getTiming() const {
	return this_timing - last_timing;
}
std::time_t TimeKeeper::getStartTime() const {
	return (this->start_time);
}

const boost::timer & TimeKeeper::getTimer() const {
	return timer;
}
}//NAMESPACE

}//NAMESPACE
