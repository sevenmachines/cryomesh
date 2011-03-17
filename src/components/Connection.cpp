/*
 * Connection.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

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
	std::cout<<"Connection::update: "<<this<<std::endl;
#endif
	// update all impulses
	this->impulses.decrementActivityTimers();
	// TODO update mesh

	// Get any finished impulses
	std::list<boost::shared_ptr<Impulse> > done_impulses = this->impulses.removeByActivityTimerValue();

	// Pass any finished impulses on to end node
	if (done_impulses.size() > 0) {
		std::map<boost::uuids::uuid, boost::shared_ptr<Node> > all_nodes = this->connector->getMutableOutputs();
		// forall in all_nodes
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::iterator it_all_nodes = all_nodes.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_all_nodes_end =
					all_nodes.end();
			while (it_all_nodes != it_all_nodes_end) {
				it_all_nodes->second->addImpulses(done_impulses);
				++it_all_nodes;
			}
		}
	}
}

const common::Connector<Connection, Node> & Connection::getConnector() const {
	return *connector;
}

common::Connector<Connection, Node> & Connection::getMutableConnector() {
	return *connector;
}

boost::shared_ptr<Impulse> Connection::add(boost::shared_ptr<Impulse> impulse) {
	// set the timer variables of added impulse
	impulse->setActivityTimer(this->activityTimer);
	return this->impulses.add(impulse);
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
		spacial::Point startp = innode.begin()->second->getPosition();
		spacial::Point endp = outnode.begin()->second->getPosition();
		double dist = startp.getDistance(endp);
		assert (dist>0);
		boost::shared_ptr<ActivityTimerDistance> temptimer(new ActivityTimerDistance(dist, activityTimer->getDelay()));
		activityTimer = temptimer;
	}
}

std::ostream& operator<<(std::ostream & os, const Connection & obj) {
	os << "Connection: " << "impulses:" << obj.getImpulses().getSize() << std::endl;
	os << "\t" << obj.getImpulses() << std::endl;
	return os;
}

}//NAMESPACE

}//NAMESPACE
