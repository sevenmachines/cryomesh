/*
 * Connection.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#define CONNECTION_DEBUG

#include "Connection.h"
#include "manager/ConnectionDatabaseObject.h"

namespace cryomesh {

namespace components {

Connection::Connection() {
	connector = boost::shared_ptr<common::Connector<Connection, Node> >(new common::Connector<Connection, Node>(1, 1));
	activityTimer = components::ActivityTimerDistance::getRandom();
}

Connection::~Connection() {
}

void Connection::update() {
#ifdef CONNECTION_DEBUG
	int pre_impulse_count = impulses.getSize();
#endif
	// update all impulses
	this->impulses.decrementActivityTimers();
	// TODO update mesh

	// Get any finished impulses

	std::list<boost::shared_ptr<Impulse> > done_impulses = this->impulses.removeByActivityTimerValue();

	// Pass any finished impulses on to end node
	if (done_impulses.size() > 0) {
		std::cout << "Connection::update: " << "size: " << done_impulses.size() << std::endl;
		std::map<boost::uuids::uuid, boost::shared_ptr<Node> > all_nodes = this->connector->getMutableOutputs();
		// forall in all_nodes
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::iterator it_all_nodes = all_nodes.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_all_nodes_end =
					all_nodes.end();
			while (it_all_nodes != it_all_nodes_end) {
#ifdef CONNECTION_DEBUG
				int pre_node_impulse_count = it_all_nodes->second->getImpulses().getSize();
#endif
				it_all_nodes->second->addImpulses(done_impulses);
#ifdef CONNECTION_DEBUG
				int post_node_impulse_count = it_all_nodes->second->getImpulses().getSize();
				std::cout << "Connection::update: " << post_node_impulse_count << "==" << pre_node_impulse_count << "+"
						<< done_impulses.size() << std::endl;
				;
				assert(post_node_impulse_count == pre_node_impulse_count+done_impulses.size());
				std::cout << "Connection::update: " << "Adding Impulse to Node '"
						<< it_all_nodes->second->getUUIDSummary() << "'" << std::endl;
				std::cout << "Connection::update: " << *this << std::endl;
#endif
				++it_all_nodes;
			}
		}
	}
#ifdef CONNECTION_DEBUG
	int post_impulse_count = impulses.getSize();
	int post_done_impulses = done_impulses.size();
	assert( post_impulse_count== pre_impulse_count - post_done_impulses);
#endif
}

const common::Connector<Connection, Node> & Connection::getConnector() const {
	return *connector;
}

common::Connector<Connection, Node> & Connection::getMutableConnector() {
	return *connector;
}

boost::shared_ptr<Impulse> Connection::add(boost::shared_ptr<Impulse> impulse) {
#ifdef CONNECTION_DEBUG
	int pre_impulses = this->impulses.getSize();
#endif
	// set the timer variables of added impulse
	impulse->setActivityTimer(this->activityTimer);
	//std::cout << "Connection::add: " << *impulse << std::endl;
	boost::shared_ptr< Impulse > added_impulse = this->impulses.add(impulse);
#ifdef CONNECTION_DEBUG
	int post_impulses = this->impulses.getSize();
	assert(post_impulses == pre_impulses +1);
#endif
	return added_impulse;
}

boost::shared_ptr<Impulse> Connection::remove(boost::shared_ptr<Impulse> impulse) {
	return this->impulses.remove(impulse);
}

boost::shared_ptr<Impulse> Connection::remove(Impulse & impulse) {
	return this->impulses.remove(impulse);
}

const ImpulseCollection & Connection::getImpulses() const {
	return (this->impulses);
}

ImpulseCollection & Connection::getMutableImpulses() {
	return (this->impulses);
}

//DEPRECATED
/**
 const ActiveImpulseCollection & Connection::getActiveImpulses() const {
 return this->activeImpulses;
 }

 ActiveImpulseCollection & Connection::getMutableActiveImpulses()  {
 return this->activeImpulses;
 }
 */
const boost::shared_ptr<components::ActivityTimerDistance> Connection::getActivityTimer() const {
	return activityTimer;
}

boost::shared_ptr<components::ActivityTimerDistance> Connection::getMutableActivityTimer() {
	return activityTimer;
}

boost::shared_ptr<manager::DatabaseObject> Connection::getDatabaseObject() const {
	const common::Cycle & cycle = common::TimeKeeper::getTimeKeeper().getCycle();
	std::string innode;
	if (this->getConnector().getInputs().size() > 0) {
		innode = this->getConnector().getInputs().begin()->second->getUUIDString();
	} else {
		innode = "NULL";
	}
	std::string outnode;
	if (this->getConnector().getOutputs().size() > 0) {
		outnode = this->getConnector().getOutputs().begin()->second->getUUIDString();
	} else {
		outnode = "NULL";
	}
	int impulse_count = this->getImpulses().getSize();
	boost::shared_ptr<manager::DatabaseObject> temp(
			new manager::ConnectionDatabaseObject(this->getUUIDString(), innode, outnode, cycle, impulse_count));
	return temp;
}

void Connection::updatePosition() {
	const std::map<boost::uuids::uuid, boost::shared_ptr<Node> > & innode = connector->getInputs();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Node> > & outnode = connector->getOutputs();
	if (innode.size() > 0 && outnode.size() > 0) {
		std::cout << "Connection::updatePosition: " << "" << std::endl;
		spacial::Point startp = innode.begin()->second->getPosition();
		spacial::Point endp = outnode.begin()->second->getPosition();
		double dist = startp.getDistance(endp);
		assert (dist>0);
		boost::shared_ptr<ActivityTimerDistance> temptimer(new ActivityTimerDistance(dist, activityTimer->getDelay()));
		activityTimer = temptimer;
		assert(activityTimer->getDelay()>ActivityTimerDistance::MIN_DISTANCE);
		assert(activityTimer->getDelay()>ActivityTimerDistance::MIN_DISTANCE);

	}
}

void Connection::connectInput(boost::shared_ptr<Node> node) {
	this->getMutableConnector().connectInput(node);
}
void Connection::connectOutput(boost::shared_ptr<Node> node) {
	this->getMutableConnector().connectOutput(node);
}

bool Connection::isPrimaryInputConnection() const {
	int input_nodes_to_connection = this->getConnector().getInputs().size();
	if (input_nodes_to_connection == 0) {
		return true;
	}
	return false;
}

bool Connection::isPrimaryOutputConnection() const {
	int output_nodes_to_connection = this->getConnector().getOutputs().size();
	if (output_nodes_to_connection == 0) {
		return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream & os, const Connection & obj) {
	std::stringstream ss;
	if (obj.isPrimaryInputConnection()) {
		ss << "(PIN) ";
	}
	if (obj.isPrimaryOutputConnection()) {
		ss << "(POUT) ";
	}

	std::string in_node;
	std::string out_node;

	if (obj.getConnector().getInputs().size() > 0) {
		in_node = obj.getConnector().getInputs().begin()->second->getUUIDSummary();
	} else {
		in_node = "PIN";
	}
	if (obj.getConnector().getOutputs().size() > 0) {
		out_node = obj.getConnector().getOutputs().begin()->second->getUUIDSummary();

	} else {
		out_node = "POUT";
	}
	os << "Connection: " << obj.getUUIDSummary() << " " << ss.str() << " node {" << in_node << " -> " << out_node
			<< "}" << " impulses:" << obj.getImpulses().getSize();
	if (obj.isDebugOn() == true) {
		if (obj.getImpulses().getSize() > 0) {
			os << std::endl << "\t" << obj.getImpulses() << std::endl;
		}
	}
	return os;
}

}//NAMESPACE

}//NAMESPACE
