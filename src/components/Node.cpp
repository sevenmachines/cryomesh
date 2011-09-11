/*
 * Node.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

//#define NODE_DEBUG

#include "Node.h"
#include "structures/Mesh.h"

#include "components/Connection.h"
#include "common/TimeKeeper.h"
#include "common/Maths.h"

namespace cryomesh {

namespace components {

const int Node::MAX_ACTIVITIES_LENGTH = 10;
const double Node::MAX_ACTIVITY_THRESHOLD = 3 * Impulse::MAX_ACTIVITY;
const double Node::MIN_ACTIVITY_THRESHOLD = 1 * Impulse::MAX_ACTIVITY;

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
		activityThreshold(MAX_ACTIVITY_THRESHOLD), connector(), impulses(),emittedImpulse(), activities(), position(0,0,0) , lastActivationState(None){
	connector = boost::shared_ptr<common::Connector<Node, Connection> >(new common::Connector<Node, Connection>());
	emittedImpulse = boost::shared_ptr<Impulse>(new Impulse());
	emittedImpulse->randomise();
	activities.setDatasetMaximumSize(MAX_ACTIVITIES_LENGTH);
}

Node::~Node() {
}

void Node::update() {
	if (isDebugOn() == true) {
		std::cout << "Node::update: " << *this << std::endl;
	}
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
#ifdef NODE_DEBUG
	std::cout<<"Node::checkFire: "<<""<<std::endl;
#endif
	//and emit impulse if required
	const ActivationState act_state = this->checkActivationState();
	if (act_state == Positive) {
#ifdef NODE_DEBUG
		std::cout << "Node::checkFire: " << "FIRE_POSITIVE" << std::endl;
#endif
		this->emitImpulsePositive();
		this->enterRecovery();
	} else if (act_state == Negative) {
#ifdef NODE_DEBUG
		std::cout << "Node::checkFire: " << "FIRE_NEGATIVE" << std::endl;
#endif
		this->emitImpulseNegative();
		this->enterRecovery();
	}
	return act_state;

}

void Node::enterRecovery(const int recovery_settings) {
#ifdef NODE_DEBUG
	int pre_impulses_size = impulses.getSize();
#endif
	if (recovery_settings & CLEAR_ALL_IMPULSES) {
		impulses.clear();
	} else if (recovery_settings & CLEAR_ACTIVE_IMPULSES) {
		impulses.clearActiveImpulses(common::TimeKeeper::getTimeKeeper().getCycle());
	}

#ifdef NODE_DEBUG
	int post_impulses_size = impulses.getSize();
	if (recovery_settings & CLEAR_ALL_IMPULSES) {
		assert(post_impulses_size==0);
	} else if (recovery_settings & CLEAR_ACTIVE_IMPULSES) {
		assert(post_impulses_size<pre_impulses_size);
	}
#endif
}

void Node::updateImpulses() {
	//std::cout<<"Node::updateImpulses: "<<"cycle:"<<common::TimeKeeper::getTimeKeeper().getCycle()<<" impulses:"<<impulses.getSize()<<std::endl;
	// drop all impulses from collection before this cycle
	impulses.clearImpulses();
}

boost::shared_ptr<Impulse> Node::addImpulse(boost::shared_ptr<Impulse> impulse) {
	// Update start to start+now
	impulse->setFirstActiveCycle(common::TimeKeeper::getTimeKeeper().getCycle());
	if (this->isDebugOn() == true) {
		std::cout << "Node::addImpulse: " << *impulse << std::endl;
	}
	boost::shared_ptr<Impulse> temp_imp = this->getMutableImpulses().add(impulse);

	return temp_imp;
}
void Node::addImpulses(std::list<boost::shared_ptr<Impulse> > impulses) {
# ifdef NODE_DEBUG
	int impulses_to_add = impulses.size();
	int node_pre_impulses_count = this->getImpulses().getSize();
#endif
	// forall in impulses
	{
		std::list<boost::shared_ptr<Impulse> >::iterator it_impulses = impulses.begin();
		const std::list<boost::shared_ptr<Impulse> >::const_iterator it_impulses_end = impulses.end();
		while (it_impulses != it_impulses_end) {
			this->addImpulse(*it_impulses);
			++it_impulses;
		}
	}
# ifdef NODE_DEBUG
	int node_post_impulses_count = this->getImpulses().getSize();
	assert(node_post_impulses_count == node_pre_impulses_count+impulses_to_add);
	std::cout << "Node::addImpulses: " << node_post_impulses_count << "=" << node_pre_impulses_count << "+"
	<< impulses_to_add << std::endl;
#endif
}

Node::ActivationState Node::checkActivationState() {
	Node::ActivationState act_state;
	// check activity on this cycle
	double current_activity = this->updateActivity();
#ifdef NODE_DEBUG
	std::cout << "Node::checkActivation: " << "Current:" << current_activity << " Threshold:" << ACTIVITY_THRESHOLD<< std::endl;
#endif
	// check activation against criteria
	if (current_activity > this->getActivityThreshold()) {
		act_state = Positive;
	} else if (current_activity < -getActivityThreshold()) {
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
	std::map < boost::uuids::uuid, boost::shared_ptr<Connection> > &objs
			= this->getMutableConnector().getMutableOutputs();
	// forall in objs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_objs = objs.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_objs_end = objs.end();
		while (it_objs != it_objs_end) {
			Connection & temp_connection = *(it_objs->second);
#ifdef NODE_DEBUG
			int pre_con_impulses = temp_connection.getImpulses().getSize();
#endif
			if (positive == true) {
				boost::shared_ptr<Impulse> temp_pulse(new Impulse(*(this->getMutableEmittedImpulse())));
				temp_connection.add(temp_pulse);
				assert(temp_pulse->getUUID() != this->getEmittedImpulse()->getUUID());
			} else {
				boost::shared_ptr<Impulse> temp_pulse(new Impulse(*(this->getMutableEmittedImpulse())));
				temp_pulse->invert();
				temp_connection.add(temp_pulse);
				assert(temp_pulse->getUUID() != this->getEmittedImpulse()->getUUID());
			}
			++it_objs;
#ifdef NODE_DEBUG
			int post_con_impulses = temp_connection.getImpulses().getSize();
			assert(post_con_impulses == pre_con_impulses +1);

			// forall in all_impulses
			{
				std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_impulses =
				temp_connection.getImpulses().getCollection().begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_impulses_end =
				temp_connection.getImpulses().getCollection().end();
				while (it_all_impulses != it_all_impulses_end) {
					assert(it_all_impulses->second->getActivityTimer()->checkConstraints());
					++it_all_impulses;
				}
			}
#endif
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

double Node::getActivityThreshold() const{
	return activityThreshold;
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
	emittedImpulse = Impulse::getRandom(0.8);
	activityThreshold = common::Maths::getRandomDouble(MIN_ACTIVITY_THRESHOLD, MAX_ACTIVITY_THRESHOLD);
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
		if (compare_doubles != 0) {
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
bool Node::isLive() {
	bool success = false;

	if (this->getImpulses().getSize() > 0) {
		success = true;
	}
	return success;
}

bool Node::isInputIsolated() const {
	if (this->getConnector().getInputs().size() < 1) {
		return true;
	}
	return false;
}

bool Node::isOutputIsolated() const {
	if (this->getConnector().getOutputs().size() < 1) {
		return true;
	}
	return false;
}

bool Node::isPrimaryInputAttachedNode() const {
	bool found_primary = false;
	if (this->isInputIsolated() == false) {

		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & all_connections =
				this->getConnector().getInputs();
		boost::shared_ptr<Connection> found_connection;
		// forall in all_connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections =
					all_connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections_end =
					all_connections.end();
			while (it_all_connections != it_all_connections_end && found_primary == false) {
				if (it_all_connections->second->isPrimaryInputConnection() == true) {
					found_primary = true;
					found_connection = it_all_connections->second;
				}
				++it_all_connections;
			}
		}
	}
	return found_primary;
}

bool Node::isPrimaryOutputAttachedNode() const {
	bool found_primary = false;
	if (this->isOutputIsolated() == false) {

		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & all_connections =
				this->getConnector().getOutputs();
		boost::shared_ptr<Connection> found_connection;
		// forall in all_connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections =
					all_connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections_end =
					all_connections.end();
			while (it_all_connections != it_all_connections_end && found_primary == false) {
				if (it_all_connections->second->isPrimaryOutputConnection() == true) {
					found_primary = true;
					found_connection = it_all_connections->second;
				}
				++it_all_connections;
			}
		}
	}
	return found_primary;
}

std::vector<boost::shared_ptr<Connection> > Node::getPrimaryInputConnections() {
	std::vector < boost::shared_ptr<Connection> > found_connections;

	const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & all_connections =
			this->getConnector().getInputs();
	// forall in all_connections
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections =
				all_connections.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections_end =
				all_connections.end();
		while (it_all_connections != it_all_connections_end) {
			int input_nodes_to_connection = it_all_connections->second->getConnector().getInputs().size();
			if (input_nodes_to_connection == 0) {
				found_connections.push_back(it_all_connections->second);
			}
			++it_all_connections;
		}
	}
	return found_connections;
}

std::vector<boost::shared_ptr<Connection> > Node::getPrimaryOutputConnections() {
	std::vector < boost::shared_ptr<Connection> > found_connections;

	const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & all_connections =
			this->getConnector().getOutputs();
	// forall in all_connections
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections =
				all_connections.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_connections_end =
				all_connections.end();
		while (it_all_connections != it_all_connections_end) {
			int output_nodes_to_connection = it_all_connections->second->getConnector().getOutputs().size();
			if (output_nodes_to_connection == 0) {
				found_connections.push_back(it_all_connections->second);
			}
			++it_all_connections;
		}
	}
	return found_connections;
}

void Node::connectInput(boost::shared_ptr<Connection> con) {
	this->getMutableConnector().connectInput(con);
}
void Node::connectOutput(boost::shared_ptr<Connection> con) {
	this->getMutableConnector().connectOutput(con);
}

void Node::updatePosition() {
	// update input connections
	{
		std::map < boost::uuids::uuid, boost::shared_ptr<Connection> > &connections = connector->getMutableInputs();
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
		std::map < boost::uuids::uuid, boost::shared_ptr<Connection> > &connections = connector->getMutableOutputs();
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
void Node::enableDebug(bool b) {
	this->setDebug(b);
}
std::ostream& operator<<(std::ostream & os, const Node & obj) {
	std::stringstream ss;
	if (obj.isPrimaryInputAttachedNode()) {
		ss << "(PIN) ";
	}
	if (obj.isPrimaryOutputAttachedNode()) {
		ss << "(POUT) ";
	}
	os << "Node: " << obj.getUUIDSummary() << " " << ss.str() << "connections:"
			<< obj.getConnector().getInputs().size() << ">" << obj.getConnector().getOutputs().size() << " impulses: "
			<< obj.getImpulses().getSize() << " activityThreshold: "<<obj.getActivityThreshold();

	if (obj.isDebugOn() == true) {
		if (obj.getImpulses().getSize() > 0) {
			os << std::endl << obj.getImpulses();
		}
		//	os<<obj.getConnector()<<std::endl;
		if (obj.getConnector().getInputs().size() > 0) {
			os << std::endl << "\t Input Connections: " << obj.getConnector().getInputs().size() << std::endl;
			obj.printConnections(os, obj.getConnector().getInputs(), "\t\t");
		}
		if (obj.getConnector().getOutputs().size() > 0) {
			os << std::endl << "\t Output Connections: " << obj.getConnector().getOutputs().size() << std::endl;
			obj.printConnections(os, obj.getConnector().getOutputs(), "\t\t");
		}
	}
	return os;
}

std::ostream & Node::printConnections(std::ostream & os,
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > & all_cons, const std::string formatter) const {
	// forall in all_cons
	{
		int count = 1;
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_cons = all_cons.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_all_cons_end =
				all_cons.end();
		while (it_all_cons != it_all_cons_end) {
			os << formatter << count << ": " << *(it_all_cons->second) << std::endl;
			++count;
			++it_all_cons;
		}
	}
	return os;
}

} //NAMESPACE

}
//NAMESPACE
