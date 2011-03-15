/*
 * Fibre.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#include "Fibre.h"
#include "components/Connection.h"
#include "Cluster.h"

#ifdef FIBRE_DEBUG
#include "common/Misc.h"
#endif

namespace cryomesh {

namespace structures {

Fibre::Fibre(boost::shared_ptr<Cluster> inputCluster, boost::shared_ptr<Cluster> outputCluster, int width) {
	this->createConnections(width);
	this->connectAllConnections(inputCluster, InputCluster);
	this->connectAllConnections(outputCluster, OutputCluster);

	if (inputCluster->getUUID() == outputCluster->getUUID()) {
		this->setType(LoopbackFibre);
	} else {
		this->setType(IntermediateFibre);
	}
}

Fibre::Fibre(boost::shared_ptr<Cluster> cluster, const FibreType & tp, int width) {
	this->setType(tp);
	this->createConnections(width);
	if (this->getType() == PrimaryInputFibre) {
		this->connectAllConnections(cluster, OutputCluster);
	} else if (this->getType() == PrimaryOutputFibre) {
		this->connectAllConnections(cluster, InputCluster);
	}
}

Fibre::~Fibre() {
	this->disconnectAllConnections();
}

void Fibre::update() {
#ifdef FIBRE_DEBUG
	std::cout << "Fibre::update: " << this << std::endl;
#endif
	// update all connections
	connections.update();
}

void Fibre::trigger() {
	std::vector<boost::shared_ptr<components::Impulse> > all_impulses;
	const unsigned int sz = this->getConnections().getSize();
	for (unsigned int i = 0; i < sz; i++) {
		all_impulses.push_back(components::Impulse::getTriggerImpulse());
	}
	assert(all_impulses.size() == sz);
	this->trigger(all_impulses);
}
void Fibre::trigger(double percentage) {
	// clamp as percentage
	double percent = std::min(percentage, 1.0);
	percent = std::max(0.0, percent);
	this->trigger(state::Pattern::getRandom(this->getWidth(), percent));
}
void Fibre::trigger(const state::Pattern & pattern) {
	std::vector<boost::shared_ptr<components::Impulse> > all_impulses;

	const std::vector<bool> & all_bools = pattern.getPattern();
	std::vector<bool>::const_iterator it_all_bools = all_bools.begin();
	std::vector<bool>::const_iterator it_all_bools_end = all_bools.end();

	boost::shared_ptr<components::Impulse> null_pulse;
	const unsigned int sz = this->getConnections().getSize();
	while (it_all_bools != it_all_bools_end) {
		if (*it_all_bools == true) {
			all_impulses.push_back(components::Impulse::getTriggerImpulse());
		} else {
			all_impulses.push_back(null_pulse);
		}
		++it_all_bools;
	}
	assert(all_impulses.size() == sz);
	this->trigger(all_impulses);
}

void Fibre::trigger(std::vector<boost::shared_ptr<components::Impulse> > & triggerImpulses) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & all_connections =
			connections.getMutableCollection();

	// if trigger impulse are short then pad
	{
		boost::shared_ptr<components::Impulse> null_pulse;
		int diff = all_connections.size() - triggerImpulses.size();
		for (int i = 0; i < diff; i++) {
			triggerImpulses.push_back(null_pulse);
		}
	}

	// forall in all_connections
	{
		std::vector<boost::shared_ptr<components::Impulse> >::iterator it_triggerimpulses = triggerImpulses.begin();
		const std::vector<boost::shared_ptr<components::Impulse> >::const_iterator it_triggerimpulses_end =
				triggerImpulses.end();
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::iterator it_all_connections =
				all_connections.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
				it_all_connections_end = all_connections.end();

		while (it_all_connections != it_all_connections_end) {
			if (*it_triggerimpulses != 0) {
				it_all_connections->second->add(*it_triggerimpulses);
			}
			++it_all_connections;
			++it_triggerimpulses;
		}
	}
}

const common::Connector<Fibre, Cluster> & Fibre::getConnector() const {
	return connector;
}

common::Connector<Fibre, Cluster> & Fibre::getMutableConnector() {
	return connector;
}
const components::ConnectionMap & Fibre::getConnections() const {
	return connections;
}

const Fibre::FibreType & Fibre::getType() const {
	return fibreType;
}

void Fibre::setType(const Fibre::FibreType & tp) {
	fibreType = tp;
}

unsigned int Fibre::getWidth() const {
	return this->getConnections().getSize();
}

const Fibre::ClusterConnectionType Fibre::isConnected(const boost::shared_ptr<Cluster> & cluster) const {
	ClusterConnectionType contype = NullCluster;
	//std::cout<<"Fibre::isConnected: "<<common::Misc::print(std::cout, cluster->getUUID())<<std::endl;
	//std::cout<<"Fibre::isConnected: "<<this->getConnector()<<std::endl;
	// check for input
	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & inputclusters = this->getConnector().getInputs();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & outputclusters =
			this->getConnector().getOutputs();

	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_input_found =
			inputclusters.find(cluster->getUUID());
	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_output_found =
			outputclusters.find(cluster->getUUID());

	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_input_end = inputclusters.end();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_output_end =
			outputclusters.end();

	bool infound = it_input_found != it_input_end;
	bool outfound = it_output_found != it_output_end;

	if ((infound == true) && (outfound == true)) { // loopback case
		contype = ClusterConnectionType::LoopbackCluster;
	} else if (infound == true) {
		contype = ClusterConnectionType::InputCluster;
	} else if (outfound == true) {
		contype = ClusterConnectionType::OutputCluster;
	}

	return contype;
}

void Fibre::createConnections(int number) {
	// disconnect original connections
	this->disconnectAllConnections();
	//clear
	connections.clear();
	//recreate
	for (int i = 0; i < number; i++) {
		boost::shared_ptr<components::Connection> tempcon(new components::Connection);
		connections.add(tempcon);
	}
}

void Fibre::disconnectAllConnections() {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & allcons =
			connections.getMutableCollection();
	// forall in allcons
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::iterator it_allcons = allcons.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_allcons_end =
				allcons.end();
		while (it_allcons != it_allcons_end) {
			it_allcons->second->getMutableConnector().disconnectAllInputs();
			it_allcons->second->getMutableConnector().disconnectAllOutputs();
			++it_allcons;
		}
	}
	this->connector.disconnectAllInputs();
	this->connector.disconnectAllOutputs();
}

void Fibre::connectAllConnections(boost::shared_ptr<Cluster> cluster, ClusterConnectionType type) {

	// get random nodes from cluster
	const int width = this->connections.getSize();
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & allconnections =
			connections.getMutableCollection();

	std::vector<boost::shared_ptr<components::Node> > nodehooks = cluster->getNodeMap().getRandomRange(width);

	// forall in nodehooks
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::iterator it_allconnections =
				allconnections.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
				it_allconnections_end = allconnections.end();

		std::vector<boost::shared_ptr<components::Node> >::iterator it_nodehooks = nodehooks.begin();
		const std::vector<boost::shared_ptr<components::Node> >::const_iterator it_nodehooks_end = nodehooks.end();

		while (it_allconnections != it_allconnections_end) {
			if (it_nodehooks == it_nodehooks_end) {
				it_nodehooks = nodehooks.begin();
			}
			if (type == InputCluster) {
				(*it_nodehooks)->getMutableConnector().connectOutput(it_allconnections->second);
				it_allconnections->second->getMutableConnector().connectInput(*it_nodehooks);
			} else if (type == OutputCluster) {
				(*it_nodehooks)->getMutableConnector().connectInput(it_allconnections->second);
				it_allconnections->second->getMutableConnector().connectOutput(*it_nodehooks);
			}
			++it_allconnections;
			++it_nodehooks;
		}
	}

	if (type == InputCluster) {
		this->connector.connectInput(cluster);
	} else if (type == OutputCluster) {
		this->connector.connectOutput(cluster);
	}

}

std::ostream& operator<<(std::ostream & os, const Fibre & obj) {
	os << "Fibre: " << obj.getConnections().getSize() << " :"
			<< obj.getConnections().getActivityPattern()->toPlusBooleanString();
	return os;
}

}//NAMESPACE

}//NAMESPACE
