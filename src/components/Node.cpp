/*
 * Node.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "Node.h"
#include "Mesh.h"

#include "components/Connection.h"
#include "common/TimeKeeper.h"
#include "common/Maths.h"

namespace cryomesh {

namespace components {

const int Node::MAX_ACTIVITIES_LENGTH = 10;
const double Node::ACTIVITY_THRESHOLD = 3 * Impulse::MAX_ACTIVITY;

const spacial::Point Node::MAX_BOUNDING_BOX_POINT = spacial::Point(100, 100, 100);

boost::shared_ptr<Node> Node::getRandom(const spacial::Point & max_point) {
	boost::shared_ptr<Node> node(new Node);
	node->randomise();
	double x = common::Maths::getRandomDouble(0, max_point.getX());
	double y = common::Maths::getRandomDouble(0, max_point.getY());
	double z = common::Maths::getRandomDouble(0, max_point.getZ());
	spacial::Point random_point(x, y, z);
	node->setPosition(random_point);
	return node;
}

Node::Node() :
	lastActivationState(None) {
	connector = boost::shared_ptr<common::Connector<Node, Connection> >(new common::Connector<Node, Connection>());
	emittedImpulse = boost::shared_ptr<Impulse>(new Impulse());
	emittedImpulse->randomise();
	activities.setDatasetMaximumSize(MAX_ACTIVITIES_LENGTH);
}

Node::~Node() {
}

void Node::update() {
	//std::cout << "Node::update: " << *this << std::endl;
	ActivationState state = this->checkFire();
	if (state != ActivationState::None) {
		if (isDebugOn() == true) {
			if (state == ActivationState::Positive) {
				std::cout << "Node::update: " << "ActivationState::Positive" << std::endl;
			} else if (state == ActivationState::Negative) {
				std::cout << "Node::update: " << "ActivationState::Negative" << std::endl;
			}
		}

		this->getMutableImpulses().clearActiveImpulses();
	}

	this->updateImpulses();

	// now check if we need to update our data object
	if (dataObject.isLoggingEnabled() == true) {
		const std::map<common::Cycle, double>::const_reverse_iterator it_rbegin = this->getActivities().rbegin();
		std::map<common::Cycle, double>::const_reverse_iterator it_rend = this->getActivities().rend();
		if (it_rbegin != it_rend) {
			dataObject.insert(it_rbegin->first.toULInt(), it_rbegin->second);
		}
	}

	lastActivationState = state;
}

void Node::forceFire() {
	this->addImpulse(Impulse::getTriggerImpulse());
}

const common::Connector<Node, Connection> & Node::getConnector() const {
	return *connector;
}

common::Connector<Node, Connection> & Node::getMutableConnector() {
	return *connector;
}

Node::ActivationState Node::checkFire() {
	//and emit impulse if required
	const ActivationState act_state = this->checkActivationState();
	if (act_state == Positive) {
		this->emitImpulsePositive();

	} else if (act_state == Negative) {
		this->emitImpulseNegative();
	}
	return act_state;

}
void Node::updateImpulses() {
	//std::cout<<"Node::updateImpulses: "<<"cycle:"<<common::TimeKeeper::getTimeKeeper().getCycle()<<" impulses:"<<impulses.getSize()<<std::endl;
	// drop all impulses from collection before this cycle
	impulses.clearImpulses();
}

boost::shared_ptr<Impulse> Node::addImpulse(boost::shared_ptr<Impulse> impulse) {
	// Update start to start+now
	impulse->setFirstActiveCycle( common::TimeKeeper::getTimeKeeper().getCycle());
	if (this->isDebugOn() == true) {
		std::cout << "Node::addImpulse: " << *impulse << std::endl;
	}
	boost::shared_ptr<Impulse>  temp_imp = this->getMutableImpulses().add(impulse);

	return temp_imp;
}
void Node::addImpulses(std::list<boost::shared_ptr<Impulse> > impulses) {
	// forall in impulses
	{
		std::list<boost::shared_ptr<Impulse> >::iterator it_impulses = impulses.begin();
		const std::list<boost::shared_ptr<Impulse> >::const_iterator it_impulses_end = impulses.end();
		while (it_impulses != it_impulses_end) {
			this->addImpulse(*it_impulses);
			++it_impulses;
		}
	}
}

Node::ActivationState Node::checkActivationState() {
	Node::ActivationState act_state;
	// check activity on this cycle
	double current_activity = this->updateActivity();
	//std::cout << "Node::checkActivation: " << "Current:" << current_activity << " Threshold:" << ACTIVITY_THRESHOLD<< std::endl;

	// check activation against criteria
	if (current_activity > ACTIVITY_THRESHOLD) {
		act_state = Positive;
	} else if (current_activity < -ACTIVITY_THRESHOLD) {
		act_state = Negative;
	} else {
		act_state = None;
	}
	return act_state;
}

void Node::emitImpulsePositive() {
	this->emitImpulse(true);
}

void Node::emitImpulseNegative() {
	this->emitImpulse(false);
}

void Node::emitImpulse(bool positive) {
	// Add impulse to all outgoing connections
	std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & objs =
			this->getMutableConnector().getMutableOutputs();
	// forall in objs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_objs = objs.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_objs_end = objs.end();
		while (it_objs != it_objs_end) {
			if (positive == true) {
				boost::shared_ptr<Impulse> temp_pulse = this->getMutableEmittedImpulse();
				it_objs->second->add(temp_pulse);
				//	std::cout << "Node::emitImpulse: " << "Positive: " << *temp_pulse << std::endl;
			} else {
				boost::shared_ptr<Impulse> temp_pulse = this->getMutableEmittedImpulse();
				temp_pulse->invert();
				//	std::cout << "Node::emitImpulse: " << "Negative: " << *temp_pulse << std::endl;
				it_objs->second->add(temp_pulse);
			}
			++it_objs;
		}
	}
}

const ImpulseCollection & Node::getImpulses() const {
	return impulses;
}

const boost::shared_ptr<Impulse> Node::getEmittedImpulse() const {
	return emittedImpulse;
}

boost::shared_ptr<Impulse> Node::getMutableEmittedImpulse() {
	return emittedImpulse;
}

ImpulseCollection & Node::getMutableImpulses() {
	return impulses;
}

const std::map<common::Cycle, double> & Node::getActivities() const {
	return activities.getMap();
}

double Node::getActivity() const {
	return (this->getActivity(common::TimeKeeper::getTimeKeeper().getCycle()));
}

double Node::getActivity(const common::Cycle & cycle) const {
	return this->getImpulses().getActivity(cycle);
}

double Node::updateActivity() {
	return (this->setActivity(getImpulses().getActivity()));
}

double Node::updateActivity(const common::Cycle & cycle) {
	return (this->setActivity(cycle, getImpulses().getActivity(cycle)));
}

double Node::setActivity(double activity) {
	return (this->setActivity(common::TimeKeeper::getTimeKeeper().getCycle(), activity));
}

double Node::setActivity(const common::Cycle & cycle, double activity) {
	return (this->addActivity(cycle, activity));
}

double Node::addActivity(common::Cycle cycle, double activity) {
	activities.insert(cycle, activity);
	return activity;
}

boost::shared_ptr<manager::DatabaseObject> Node::getDatabaseObject() const {
	std::map<common::Cycle, double>::const_iterator it_act = this->getActivities().begin();
	unsigned long int cycle = common::TimeKeeper::getTimeKeeper().getCycle().toULInt();
	double activity = this->getActivity();

	boost::shared_ptr<manager::DatabaseObject> temp(
			new manager::NodeDatabaseObject(this->getUUIDString(), this->getPosition(), cycle, activity));
	return temp;
}

const spacial::Point & Node::getPosition() const {
	return position;
}

void Node::setPosition(const spacial::Point & new_position) {
	this->position = spacial::Point(new_position);
	updatePosition();
}

Node::ActivationState Node::getLastActivationState() const {
	return lastActivationState;
}

void Node::randomise() {
	emittedImpulse = Impulse::getRandom();
}

bool Node::isTriggered(ActivationState state) {
	bool success = false;
	if (state == Node::None) {
		success = (this->getLastActivationState() != Node::None);
	} else {
		success = (this->getLastActivationState() == state);
	}
	return success;
}

bool Node::isActive(const ActivationState state) {
	bool success = false;
	double act = this->getActivity();
	if (state == Node::None) {
		int compare_doubles = common::Maths::compareDoubles(act, 0);
		if (  compare_doubles != 0 ) {
			success = true;
		}
	} else if (state == Node::Positive) {
		if (act > 0) {
			success = true;
		}
	} else if (state == Node::Negative) {
		if (act < 0) {
			success = true;
		}
	}
	return success;
}
bool Node::isLive(){
	bool success = false;

	if (this->getImpulses().getSize() > 0){
		success =  true;
	}
	return success;
}

void Node::updatePosition() {
	// update input connections
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & connections = connector->getMutableInputs();
		// forall in connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::iterator it_connections = connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_connections_end =
					connections.end();
			while (it_connections != it_connections_end) {
				it_connections->second->updatePosition();
				++it_connections;
			}
		}
	}
	// update output connections
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & connections = connector->getMutableOutputs();
		// forall in connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::iterator it_connections = connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_connections_end =
					connections.end();
			while (it_connections != it_connections_end) {
				it_connections->second->updatePosition();
				++it_connections;
			}
		}
	}
}

std::ostream& operator<<(std::ostream & os, const Node & obj) {
	os << "Node: " << "connections:" << obj.getConnector().getInputs().size() << ">"
			<< obj.getConnector().getOutputs().size() << " " << obj.getImpulses();
	return os;
}

} //NAMESPACE

}
//NAMESPACE
