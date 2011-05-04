/*
 * ActivityTimerDistance.cpp
 *
 *  Created on: 14 Feb 2011
 *      Author: niall
 */

#define ACTIVITYTIMERDISTANCE_DEBUG

#include "ActivityTimerDistance.h"
#include "common/Maths.h"

namespace cryomesh {

namespace components {

const double ActivityTimerDistance::MIN_DECREMENT_FRACTION = 0.1;
const double ActivityTimerDistance::MAX_DECREMENT_FRACTION = 1;
const double ActivityTimerDistance::MIN_DISTANCE = 1.0;
const double ActivityTimerDistance::MAX_DISTANCE = 100.0;

boost::shared_ptr<ActivityTimerDistance> ActivityTimerDistance::getRandom() {
	double rand_dist = common::Maths::getRandomDouble(MIN_DISTANCE, MAX_DISTANCE);
	// scale decrement as percentage of distance

	//double rand_dec = rand_dist * common::Maths::getRandomDouble(MIN_DECREMENT_FRACTION, MAX_DECREMENT_FRACTION);
	double rand_dec = MIN_DECREMENT_FRACTION;
	boost::shared_ptr<ActivityTimerDistance> rand_acttimer(new ActivityTimerDistance(rand_dist, rand_dec));

	rand_acttimer->checkConstraints();
	//std::cout<<"ActivityTimerDistance::getRandom: "<<*rand_acttimer<<std::endl;
	return rand_acttimer;
}

ActivityTimerDistance::ActivityTimerDistance() :
	distance(0), distance_remaining(0), decrement(0) {
}

ActivityTimerDistance::ActivityTimerDistance(double dist, double dec) :
	distance(dist), distance_remaining(dist), decrement(dec) {
}

ActivityTimerDistance::~ActivityTimerDistance() {
}

ActivityTimerDistance & ActivityTimerDistance::operator=(const ActivityTimerDistance & obj) {
	distance = obj.distance;
	distance_remaining = distance;
	decrement = obj.decrement;
	return *this;
}

bool ActivityTimerDistance::operator<(const ActivityTimerDistance & obj) const {
	return (this->distance < obj.distance);
}

bool ActivityTimerDistance::operator>(const ActivityTimerDistance & obj) const {
	return (this->distance_remaining > obj.distance_remaining);
}

ActivityTimerDistance & ActivityTimerDistance::operator--() {
	this->distance_remaining -= this->decrement;
	return *this;
}

ActivityTimerDistance ActivityTimerDistance::operator--(int) {
	ActivityTimerDistance old = *this;
	--(*this);
	return old;
}

const ActivityTimerDistance ActivityTimerDistance::operator+(const ActivityTimerDistance & obj) const {
	ActivityTimerDistance temp = *this;
	temp += obj;
	return temp;
}

ActivityTimerDistance & ActivityTimerDistance::operator+=(const ActivityTimerDistance & obj) {
	this->decrement += obj.decrement;
	this->distance += obj.distance;
	this->distance_remaining += obj.distance_remaining;

	return *this;
}

double ActivityTimerDistance::getDelay() const {
	return distance_remaining;
}

double ActivityTimerDistance::getStartingDelay() const {
	return distance;
}

double ActivityTimerDistance::getDecrement() const {
	return decrement;
}

void ActivityTimerDistance::reset() {
	distance_remaining = distance;
}
std::ostream & ActivityTimerDistance::print(std::ostream & os) const {
	std::cout << "ActivityTimerDistance: " << "distance=" << this->getDelay() <<"/"<<this->getStartingDelay()<< " decrement=" << this->getDecrement()
			<< std::endl;
	return os;
}

bool ActivityTimerDistance::checkConstraints()const{
	bool decrement_good = (this->getDecrement()<this->getStartingDelay());
#ifdef ACTIVITYTIMERDISTANCE_DEBUG
	if (decrement_good ==false){
		std::cout<<"ActivityTimerDistance::checkConstraints: "<<this->getDecrement()<<">"<<this->getStartingDelay()<<"this->getDecrement()>this->getStartingDelay()"<<std::endl;
	}
#endif
	bool delay_good = (this->getStartingDelay()>ActivityTimerDistance::MIN_DISTANCE);
#ifdef ACTIVITYTIMERDISTANCE_DEBUG
	if (delay_good ==false){
		std::cout<<"ActivityTimerDistance::checkConstraints: "<<this->getStartingDelay()<<">"<<MIN_DISTANCE<<"this->getStartingDelay()>MIN_DISTANCE"<<std::endl;
	}
#endif
	return decrement_good && delay_good;
}
}//NAMESPACE

}//NAMESPACE
