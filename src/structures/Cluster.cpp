/*
 * Cluster.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#include "Cluster.h"
#include <list>
#include <algorithm>
#include "common/Maths.h"
#include "Fibre.h"

namespace cryomesh {

namespace structures {

const double Cluster::SELF_CONNECTED_NODES_FRACTION = 0.1;

Cluster::Cluster() :
	common::Spacial(true) , energy(0){
}

Cluster::Cluster(int nodeCount, int connectivity) :
	common::Spacial(true) , energy(0){
	this->createNodes(nodeCount);
	this->createConnectivity(connectivity);
}

Cluster::~Cluster() {
}

void Cluster::update() {
	//std::cout << "Cluster::update: " << this << std::endl;
	// update nodes
	nodes.update();
	// update connections
	connections.update();
}

double Cluster::getEnergy() const {
	return energy;
}
void Cluster::setEnergy(double total_energy) {
	energy = total_energy;
}

void Cluster::warpMesh() {
	std::cout << "Cluster::warpMesh: " << "TODO" << std::endl;
}

void Cluster::createNodes(const int number) {
	for (int i = 0; i < number; i++) {
		boost::shared_ptr<components::Node> tempnode = components::Node::getRandom();
		nodes.add(tempnode);
	}
}

void Cluster::createConnectivity(const int connectivity) {
	this->updateConnectivity(connectivity, AsIncrement);
}

void Cluster::updateConnectivity(const int connectivity, ValueTypeSpecifier asValue) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & allnodes = nodes.getMutableCollection();
	std::vector<boost::shared_ptr<components::Node> > shufflednodes = nodes.getObjectList();
	random_shuffle(shufflednodes.begin(), shufflednodes.end());

	assert (allnodes.size() == shufflednodes.size());

	// forall in allnodes
	{
		std::vector<boost::shared_ptr<components::Node> >::const_iterator it_shufflednodes = shufflednodes.begin();
		const std::vector<boost::shared_ptr<components::Node> >::const_iterator it_shufflednodes_end =
				shufflednodes.end();

		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_allnodes =
				allnodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_allnodes_end =
				allnodes.end();
		while (it_allnodes != it_allnodes_end) {

			if (asValue == AsIncrement) {
				for (int i = 0; i < connectivity; i++) {
					// add connection
					if (it_shufflednodes == it_shufflednodes_end) {
						it_shufflednodes = shufflednodes.begin();
					}

					bool dont_self_connect = false;
					bool same_node = it_allnodes->second->getUUID() == (*it_shufflednodes)->getUUID();
					if (same_node == true && shufflednodes.size() > 1) {
						dont_self_connect = common::Maths::getRandomBool(Cluster::SELF_CONNECTED_NODES_FRACTION);
					}

					if (same_node == true && dont_self_connect == false) {
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					} else if (same_node == true && dont_self_connect == true) {
						// add connection
						++it_shufflednodes;
						if (it_shufflednodes == it_shufflednodes_end) {
							it_shufflednodes = shufflednodes.begin();
						}
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);

					} else {
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					}
					++it_shufflednodes;
				}

			} else if (asValue == AsMinumum) {
				for (int i = it_allnodes->second->getConnector().getOutputs().size(); i < connectivity; i++) {
					// add connection
					if (it_shufflednodes == it_shufflednodes_end) {
						it_shufflednodes = shufflednodes.begin();
					}

					bool dont_self_connect = false;
					bool same_node = it_allnodes->second->getUUID() == (*it_shufflednodes)->getUUID();
					if (same_node == true && shufflednodes.size() > 1) {
						dont_self_connect = common::Maths::getRandomBool(Cluster::SELF_CONNECTED_NODES_FRACTION);
					}
					if (same_node == true && dont_self_connect == false) {
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					} else if (same_node == true && dont_self_connect == true) {
						// add connection
						++it_shufflednodes;
						if (it_shufflednodes == it_shufflednodes_end) {
							it_shufflednodes = shufflednodes.begin();
						}
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);

					} else {
						this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					}
					++it_shufflednodes;
				}

			}
			++it_allnodes;
		}
	}
}
void Cluster::createConnection(boost::shared_ptr<components::Node> nodeStart,
		boost::shared_ptr<components::Node> nodeEnd, int connectivity) {

	for (int i = 0; i < connectivity; i++) {

		boost::shared_ptr<components::Connection> tempcon(new components::Connection);

		tempcon->getMutableConnector().connectInput(nodeStart);
		tempcon->getMutableConnector().connectOutput(nodeEnd);

		nodeStart->getMutableConnector().connectOutput(tempcon);
		nodeEnd->getMutableConnector().connectInput(tempcon);

		connections.add(tempcon);

	}
}

const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & Cluster::getNodes() const {
	return nodes.getCollection();
}

const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & Cluster::getConnections() const {
	return connections.getCollection();
}

const common::Connector<Cluster, Fibre> & Cluster::getConnector() const {
	return connector;
}

common::Connector<Cluster, Fibre> & Cluster::getMutableConnector() {
	return connector;
}

const components::NodeMap & Cluster::getNodeMap() const {
	return nodes;
}
components::NodeMap & Cluster::getMutableNodeMap() {
	return nodes;
}

int Cluster::getTriggeredNodeCount(const int indicator) const {
	int return_count = 0;
	int count = 0;
	int positive_count = 0;
	int negative_count = 0;
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = this->getNodes();
	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {
			if (it_all_nodes->second->getLastActivationState() != components::Node::None) {
				++count;
			}

			if (it_all_nodes->second->getLastActivationState() == components::Node::Positive) {
				++positive_count;
			} else if (it_all_nodes->second->getLastActivationState() == components::Node::Negative) {
				++negative_count;
			}
			++it_all_nodes;
		}
	}
	if (indicator > 0) {
		return_count = positive_count;
	} else if (indicator < 0) {
		return_count = negative_count;
	} else {
		return_count = count;
	}
	return return_count;
}

int Cluster::getActiveNodeCount(const int indicator) const {
	int return_count = 0;
	int count = 0;
	int positive_count = 0;
	int negative_count = 0;
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = this->getNodes();
	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {
			double temp_act = it_all_nodes->second->getActivity();
			//	std::cout << "Cluster::getActiveNodeCount: " << "indicator: " << indicator << " temp_act: " << temp_act
			//		<< std::endl;
			if (indicator > 0) {
				if (temp_act > 0)
					//		std::cout << "Cluster::getActiveNodeCount: " << "POS: " << temp_act << std::endl;
					++positive_count;
			} else if (indicator < 0) {
				if (temp_act < 0)
					//		std::cout << "Cluster::getActiveNodeCount: " << "NEG: " << temp_act << std::endl;
					++negative_count;
			} else if (indicator == 0) {
				//std::cout<<"Cluster::getActiveNodeCount: "<<""<<std::endl;
				if (temp_act < 0 || temp_act > 0)
					//std::cout << "Cluster::getActiveNodeCount: " << "ANY: " << it_all_nodes->second->getActivity() << std::endl;
					++count;
			}
			++it_all_nodes;
		}
	}
	if (indicator > 0) {
		return_count = positive_count;
	} else if (indicator < 0) {
		return_count = negative_count;
	} else {
		return_count = count;
	}
	return return_count;
}

int Cluster::getLiveNodeCount() const {
	int count = 0;
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = this->getNodes();
	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {

			if (it_all_nodes->second->getImpulses().getSize() > 0) {
				++count;
			}
			++it_all_nodes;
		}
	}
	return count;
}
std::ostream& operator<<(std::ostream & os, const Cluster & obj) {
	os << "Cluster: " << "nodes:" << obj.getNodes().size() << " connections:" << obj.getConnections().size()
			<< std::endl;
	os << obj.getNodeMap() << std::endl;
	return os;
}

}//NAMESPACE

}//NAMESPACE
