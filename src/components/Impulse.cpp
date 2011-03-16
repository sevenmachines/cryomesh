/*
 * Impulse.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "Impulse.h"
#include "ActivityTimerDistance.h"
#include "common/Maths.h"
#include "common/TimeKeeper.h"
#include <algorithm>

using namespace cryomesh::common;
namespace cryomesh {

namespace components {

const double Impulse::FORCED_TRIGGER_ACTIVITY = 1000;
const double Impulse::MAX_ACTIVITY = 1;
const double Impulse::MIN_ACTIVITY = -1;
const int Impulse::MAX_ACTIVITY_LENGTH = 10;
const int Impulse::MIN_ACTIVITY_LENGTH = 1;

// statics
boost::shared_ptr<Impulse> Impulse::getTriggerImpulse(){
	boost::shared_ptr< Impulse > temp_pulse(new Impulse(Impulse::FORCED_TRIGGER_ACTIVITY, Impulse::MIN_ACTIVITY_LENGTH, TimeKeeper::getTimeKeeper().getCycle())) ;
	return temp_pulse;
}

boost::shared_ptr<Impulse> Impulse::getRandom(){
	boost::shared_ptr< Impulse > temp_pulse(new Impulse);
	temp_pulse->randomise();
	return temp_pulse;
}

Impulse::Impulse() {
	// set start to current time
	this->generateCurve(0, MIN_ACTIVITY_LENGTH);
	setFirstActiveCycle(TimeKeeper::getTimeKeeper().getCycle());
		boost::shared_ptr<ActivityTimerDistance> tempact(new ActivityTimerDistance(1,1));
			this->setActivityTimer( tempact );
}

Impulse::Impulse(const double max_y, const int length, const common::Cycle & startCycle) {
	this->generateCurve(max_y, std::max(length, MIN_ACTIVITY_LENGTH));
	// set start to current time
	setFirstActiveCycle(startCycle);
	boost::shared_ptr<ActivityTimerDistance> tempact(new ActivityTimerDistance(1,1));
	this->setActivityTimer( tempact );

}

Impulse::Impulse(const double max_y, const int length, const common::Cycle & startCycle, boost::shared_ptr< ActivityTimerDistance > timer) {
	this->generateCurve(max_y, std::max(length, MIN_ACTIVITY_LENGTH));
	// set start to current time
	setFirstActiveCycle(startCycle);
	this->setActivityTimer( timer );

}

Impulse::~Impulse() {
}

void Impulse::randomise() {
	double maxy = common::Maths::getRandomDouble(MIN_ACTIVITY, MAX_ACTIVITY);
	double length = common::Maths::getRandomInteger(MIN_ACTIVITY_LENGTH, MAX_ACTIVITY_LENGTH);
	this->generateCurve(maxy, length);
	lastActiveCycle = firstActiveCycle + this->getCollection().size();
	boost::shared_ptr< ActivityTimerDistance > tempact = ActivityTimerDistance::getRandom() ;
	this->setActivityTimer(tempact );
}

bool Impulse::isActive() const {
	return this->isActive(common::TimeKeeper::getTimeKeeper().getCycle());
}

bool Impulse::isActive(const common::Cycle & cycle) const {
	return this->isActive(cycle, cycle);
}

bool Impulse::isActive(const common::Cycle &startCycle, const common::Cycle &endCycle) const {
	if ((firstActiveCycle <= startCycle && lastActiveCycle >= endCycle) //spans interval
			|| (firstActiveCycle >= startCycle && firstActiveCycle <= endCycle) // start is in interval
			|| (lastActiveCycle >= startCycle && lastActiveCycle <= endCycle)) { // end is in interval
		return true;
	}
	return false;
}

double Impulse::getActivity() const {
	return this->getActivity(common::TimeKeeper::getTimeKeeper().getCycle());
}

double Impulse::getActivity(Cycle cycle) const {
	//	std::cout << "Impulse::getActivity: " << "cycle: " << cycle << " (" << firstActiveCycle << "," << lastActiveCycle
	//		<< ")" << std::endl;
	if (cycle < this->getFirstActiveCycle() || cycle > this->getLastActiveCycle()) {
		return 0;
	}

	const std::list<double> & allacts = this->getCollection();
	std::list<double>::const_iterator it_allacts = allacts.begin();
	const std::list<double>::const_iterator it_allacts_end = allacts.end();

	long int index = (cycle.toLInt() - firstActiveCycle.toLInt());
	long int end_index = (lastActiveCycle.toLInt() - cycle.toLInt());
	assert(index>=0);
	for (int i = 0; i < index && i < end_index; i++) {
#ifdef IMPULSE_DEBUG
		std::cout << "Impulse::getActivity: " << "i:" << i << " (" << index << "," << end_index << ") " << " = "
		<< *it_allacts << std::endl;
#endif
		++it_allacts;
	}
	assert(it_allacts!=it_allacts_end);
	return *it_allacts;
}

double Impulse::getActivityMaximum() const {
	return (this->getMaximum());
}

double Impulse::getActivityMinimum() const {
	return (this->getMinimum());
}

Impulse & Impulse::invert() {
	SimpleCollection::invert();
	return *this;
}

Cycle Impulse::getFirstActiveCycle() const {
	return firstActiveCycle;
}

void Impulse::setFirstActiveCycle(const Cycle cycle) {
	//long int cycle_int = cycle.toLInt();
	//long int pre_fac = firstActiveCycle.toLInt();
	//long int pre_lac = lastActiveCycle.toLInt();
	//long int pre_col_sz = this->getCollection().size();
	firstActiveCycle = cycle;
	lastActiveCycle = firstActiveCycle + this->getCollection().size();

	//long int post_fac = firstActiveCycle.toLInt();
	//long int post_lac = lastActiveCycle.toLInt();
	//long int post_col_sz = this->getCollection().size();
	//bool pause = true;
}

Cycle Impulse::getLastActiveCycle() const {
	return lastActiveCycle;
}

const std::list<double> & Impulse::getActivities() const {
	return (this->getCollection());
}

const boost::shared_ptr<ActivityTimerDistance> Impulse::getActivityTimer() const {
	return activityTimer;
}

boost::shared_ptr<ActivityTimerDistance> Impulse::getMutableActivityTimer() {
	return activityTimer;
}

void Impulse::setActivityTimer(const boost::shared_ptr< ActivityTimerDistance > timer){
	activityTimer = boost::shared_ptr<ActivityTimerDistance> (new ActivityTimerDistance(*timer));
}

const Impulse Impulse::operator+(const Impulse & obj) const {
	Impulse imp = *this;
	imp += obj;
	return imp;
}

Impulse & Impulse::operator+=(const Impulse & obj) {
	const std::list<double> all_objs(obj.getCollection());
	std::list<double> & this_objs = this->getMutableCollection();
	//int allsz_old = all_objs.size();
	//int thissz_old = this_objs.size();

	// make collections bug enough to merge
	Cycle obj_fac = obj.getFirstActiveCycle();
	Cycle this_fac = this->getFirstActiveCycle();
	Cycle obj_lac = obj.getLastActiveCycle();
	Cycle this_lac = this->getLastActiveCycle();

	//long int obj_fac_int = obj.getFirstActiveCycle().toLInt();
	//long int this_fac_int = this->getFirstActiveCycle().toLInt();
	//long int obj_lac_int = obj.getLastActiveCycle().toLInt();
	//long int this_lac_int = this->getLastActiveCycle().toLInt();

	long int first_diff = (this_fac.toULInt() - obj_fac.toULInt());
	long int last_diff = (obj_lac.toULInt() - this_lac.toULInt());

	double pad = 0.0;
	this->pad(first_diff, last_diff, pad);
	//int thissz_new = this_objs.size();
	std::list<double>::iterator it_this_objs = this_objs.begin();
	const std::list<double>::const_iterator it_this_objs_end = this_objs.end();
	std::list<double>::const_iterator it_all_objs = all_objs.begin();
	const std::list<double>::const_iterator it_all_objs_end = all_objs.end();

	int count_before_obj = first_diff;
	int count = 0;
	while (it_this_objs != it_this_objs_end) {
		//std::cout<<"Impulse::operator +=: "<<"count:"<<count<<" count_before_obj: "<<count_before_obj<<std::endl;
		//	std::cout<<"Impulse::operator +=: "<<"this:" <<*it_this_objs;
		if (count_before_obj > 0 && it_all_objs != it_all_objs_end) {
			//	std::cout<<"+"<<*it_all_objs;
			*it_this_objs += *it_all_objs;
			//	std::cout<<"="<<*it_all_objs;
			++it_all_objs;
		}
		//std::cout <<std::endl;

		++count;
		++count_before_obj;
		++it_this_objs;
	}

	// set cycle bounds
	if (obj.getFirstActiveCycle() < this->getFirstActiveCycle()) {
		this->setFirstActiveCycle(obj.getFirstActiveCycle());
	}

	return *this;
}

Impulse & Impulse::operator=(const Impulse & obj) {
	if (*this == obj) {
		return *this;
	}

	this->getMutableCollection().clear();
	const std::list<double> & all_objs = obj.getCollection();
	// forall in all_objs
	{
		typename std::list<double>::const_iterator it_all_objs = all_objs.begin();
		const typename std::list<double>::const_iterator it_all_objs_end = all_objs.end();
		while (it_all_objs != it_all_objs_end) {
			this->getMutableCollection().push_back(*it_all_objs);
			++it_all_objs;
		}
	}
	this->setFirstActiveCycle(obj.getFirstActiveCycle());
	boost::shared_ptr< ActivityTimerDistance > temptimer(obj.getActivityTimer());
	this->setActivityTimer(temptimer);
	return *this;
}

bool Impulse::operator==(const Impulse & obj) const {
	//	std::cout<<"Impulse::operator ==: "<<""<<std::endl;
	//	std::cout<< *this <<std::endl;
	//	std::cout<< obj <<std::endl;
	if (this->getFirstActiveCycle() != obj.getFirstActiveCycle()) {
		return false;
	}
	if (this->getLastActiveCycle() != obj.getLastActiveCycle()) {
		return false;
	}

	const std::list<double> & all_objs = obj.getCollection();
	const std::list<double> & this_objs = this->getCollection();

	if (all_objs.size() != this_objs.size()) {
		return false;
	}

	// forall in all_objs
	{
		typename std::list<double>::const_iterator it_all_objs = all_objs.begin();
		const typename std::list<double>::const_iterator it_all_objs_end = all_objs.end();
		typename std::list<double>::const_iterator it_this_objs = this_objs.begin();
		const typename std::list<double>::const_iterator it_this_objs_end = this_objs.end();

		while (it_all_objs != it_all_objs_end && it_this_objs != it_this_objs_end) {
			if (common::Maths::compareDoubles(*it_all_objs, *it_this_objs)) {
				return false;
			}
			++it_this_objs;
			++it_all_objs;
		}
	}
	//std::cout << "Impulse::operator ==: DEBUG: " << "TRUE" << std::endl;
	return true;
}

bool Impulse::operator!=(const Impulse &obj) const {
	return !(*this == obj);
}

std::ostream& operator<<(std::ostream & os, const Impulse & obj) {
	os << "SimpleCollection: " << &obj << " objects: { ";
	// forall in objects
	{
		std::list<double>::const_iterator it_objects = obj.getCollection().begin();
		const std::list<double>::const_iterator it_objects_end = obj.getCollection().end();
		while (it_objects != it_objects_end) {
			os << *it_objects << " ";
			++it_objects;
		}
	}
	os << "}";
	os << " start/end: (" << obj.getFirstActiveCycle() << "," << obj.getLastActiveCycle() << ")";
	return os;
}

} //NAMESPACE

}//NAMESPACE
