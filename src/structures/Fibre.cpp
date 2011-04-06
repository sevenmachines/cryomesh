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

components::ConnectionMap & Fibre::getMutableConnections() {
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

const std::pair<int, int> Fibre::countConnections(
		const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters) const {
	int input_count = 0;
	int output_count = 0;
	// forall in all_clusters
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
				all_clusters.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
				all_clusters.end();
		while (it_all_clusters != it_all_clusters_end) {
			const boost::shared_ptr< Cluster > cluster = it_all_clusters->second;
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & inputclusters =
					this->getConnector().getInputs();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & outputclusters =
					this->getConnector().getOutputs();

			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_input_found =
					inputclusters.find(cluster->getUUID());
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_output_found =
					outputclusters.find(cluster->getUUID());

			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_input_end =
					inputclusters.end();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_output_end =
					outputclusters.end();

			bool infound = it_input_found != it_input_end;
			bool outfound = it_output_found != it_output_end;

			if (infound == true) {
				++input_count;
			}
			if (outfound == true) {
				++output_count;
			}
			++it_all_clusters;
		}
	}
	return std::pair<int, int>(input_count, output_count);
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
		std::cout << "Fibre::isConnected: " << "LoopbackCluster" << std::endl;
	} else if (infound == true) {
		contype = ClusterConnectionType::InputCluster;
		std::cout << "Fibre::isConnected: " << "InputCluster" << std::endl;
	} else if (outfound == true) {
		contype = ClusterConnectionType::OutputCluster;
		std::cout << "Fibre::isConnected: " << "OutputCluster" << std::endl;
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
boost::shared_ptr<state::Pattern> Fibre::getNodesPattern(
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > all_nodes) const {
	// get all connections
	std::vector<bool> firing_pattern;
	int node_count = 0;

	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {
			// if firing state is positive (Ignoring negative firing states
			if (it_all_nodes->second->getLastActivationState() == components::Node::Positive) {
				firing_pattern.push_back(true);
			} else {
				firing_pattern.push_back(false);
			}
			++node_count;
			++it_all_nodes;
		}
	}
	// generate pattern
	boost::shared_ptr<state::Pattern> pattern(new state::Pattern(firing_pattern));
	assert(pattern->getWidth() == node_count);
	return pattern;
}

boost::shared_ptr<state::Pattern> Fibre::getInputNodesPattern() const {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > all_nodes = this->getInputNodes();
	return this->getNodesPattern(all_nodes);
}
boost::shared_ptr<state::Pattern> Fibre::getOutputNodesPattern() const {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > all_nodes = this->getOutputNodes();
	return this->getNodesPattern(all_nodes);
}

const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > Fibre::getInputNodes() const {
	return (this->getNodes(InputCluster));
}

const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > Fibre::getOutputNodes() const {
	return (this->getNodes(InputCluster));
}

const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > Fibre::getNodes(
		const ClusterConnectionType type) const {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > collected_nodes;
	if (connections.getCollection().size() > 0) {
		int node_count = 0;
		// get connections
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & connections =
				this->connections.getCollection();
		// forall in connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_connections =
					connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
					it_connections_end = connections.end();
			while (it_connections != it_connections_end) {
				// for each connection, get all the nodes (in practice there should only be one)
				if (type == InputCluster) {
					const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & nodes =
							it_connections->second->getConnector().getInputs();
					// forall in nodes
					{
						std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_nodes =
								nodes.begin();
						const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator
								it_nodes_end = nodes.end();
						while (it_nodes != it_nodes_end) {
							collected_nodes[it_nodes->first] = it_nodes->second;
							++node_count;
							++it_nodes;
						}
					}
				} else if (type == OutputCluster) {
					const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & nodes =
							it_connections->second->getConnector().getOutputs();
					// forall in nodes
					{
						std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_nodes =
								nodes.begin();
						const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator
								it_nodes_end = nodes.end();
						while (it_nodes != it_nodes_end) {
							collected_nodes[it_nodes->first] = it_nodes->second;
							++node_count;
							++it_nodes;
						}
					}
				} else {
					std::cout << "Fibre::getNodes: " << "WARNING: Ignoring unknown type" << std::endl;
				}
				++it_connections;
			}
		}
	} else {
		std::cout << "Fibre::getNodes: " << "WARNING: No connections in fibre" << std::endl;
	}
	return collected_nodes;
}

void Fibre::forceFireInputNodes(const state::Pattern & pattern) {
	this->forceFireNodes(pattern, this->getInputNodes());
}

void Fibre::forceFireOutputNodes(const state::Pattern & pattern) {
	this->forceFireNodes(pattern, this->getOutputNodes());
}

void Fibre::forceFireNodes(const state::Pattern & pattern,
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > nodes) {
	// pattern should be the same size as nodes
	{
		int pat_sz = pattern.getSize();
		int nodes_sz = nodes.size();
		if (pat_sz != nodes_sz) {
			std::cout << "Fibre::forceFireNodes: " << "WARNING: Pattern and Nodes size mismatch..." << "pattern: "
					<< pat_sz << " nodes: " << nodes_sz << std::endl;
		}
	}
	// forall in nodes
	{
		std::vector<bool> pat_bool = pattern.getPattern();
		std::vector<bool>::const_iterator it_pat_bool = pat_bool.begin();
		const std::vector<bool>::const_iterator it_pat_bool_end = pat_bool.end();

		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_nodes = nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_nodes_end =
				nodes.end();

		while (it_nodes != it_nodes_end && it_pat_bool != it_pat_bool_end) {
			if (*it_pat_bool == true) {
				it_nodes->second->forceFire();
			}
			++it_pat_bool;
			++it_nodes;
		}
	}
}

std::ostream& operator<<(std::ostream & os, const Fibre & obj) {
	os << "Fibre: " << obj.getConnections().getSize() << " :"
			<< obj.getConnections().getActivityPattern()->toPlusBooleanString();
	return os;
}

}//NAMESPACE

}//NAMESPACE
