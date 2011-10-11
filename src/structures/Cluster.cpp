/*
 * Cluster.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

//#define CLUSTER_DEBUG
#include "Cluster.h"
#include "manipulators/ClusterArchitect.h"
#include <list>
#include <algorithm>
#include "common/Maths.h"
#include "Fibre.h"

namespace cryomesh {

namespace structures {

const double Cluster::SELF_CONNECTED_NODES_FRACTION = 0.1;

Cluster::Cluster() :
		spacial::Spacial(true), energy(0), nodes(), connections(), mesh(
				boost::shared_ptr<NodeMesh>(new NodeMesh(*this))), connector(), clusterArchitect(new manipulators::ClusterArchitect(*this))  {
}

Cluster::Cluster(int nodeCount, int connectivity, const spacial::Point bounding_box) :
		spacial::Spacial(bounding_box, true), energy(0), nodes(), connections(), mesh(
				boost::shared_ptr<NodeMesh>(new NodeMesh(*this))), connector(), clusterArchitect(new manipulators::ClusterArchitect(*this))  {
	clusterArchitect->createRandomNodes(nodeCount, connectivity);
}

Cluster::Cluster(int nodeCount, int connectivity) :
		spacial::Spacial(true), energy(0) , nodes(), connections(), mesh(
				boost::shared_ptr<NodeMesh>(new NodeMesh(*this))), connector() , clusterArchitect(new manipulators::ClusterArchitect(*this)) {
	clusterArchitect->createRandomNodes(nodeCount, connectivity);
}

Cluster::~Cluster() {
}

void Cluster::update() {
	//std::cout << "Cluster::update: " << this << std::endl;
	// update nodes
	nodes.update();
	// update connections
	connections.update();
	mesh->update();
}

double Cluster::getEnergy() const {
	return energy;
}
void Cluster::setEnergy(double total_energy) {
	energy = total_energy;
}

const boost::shared_ptr< manipulators::ClusterArchitect  > Cluster::getClusterArchitect() const{
	return clusterArchitect;
}

boost::shared_ptr< manipulators::ClusterArchitect  > Cluster::getMutableClusterArchitect(){
	return clusterArchitect;
}

void Cluster::warpNodes() {
	mesh->warpNodes();
}


void Cluster::createConnectivity(const int connectivity) {
	this->updateConnectivity(connectivity, AsIncrement);
}

void Cluster::updateConnectivity(const int connectivity, ValueTypeSpecifier asValue) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & allnodes = nodes.getMutableCollection();
	std::vector<boost::shared_ptr<components::Node> > shufflednodes = nodes.getObjectList();
	random_shuffle(shufflednodes.begin(), shufflednodes.end());

#ifdef CLUSTER_DEBUG
	std::cout << "Cluster::updateConnectivity: " << "allnodes: " << allnodes.size() << " shuffled: "
	<< shufflednodes.size() << std::endl;
#endif

	assert(allnodes.size() == shufflednodes.size());

	// forall in allnodes
	{
#ifdef CLUSTER_DEBUG
		int nodes_count = 0;
		int shuffled_count = 0;
#endif
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
#ifdef CLUSTER_DEBUG
						std::cout << "Cluster::updateConnectivity: " << nodes_count << " reshuffling" << std::endl;
#endif
					}

					bool dont_self_connect = false;
					bool same_node = it_allnodes->second->getUUID() == (*it_shufflednodes)->getUUID();
					if (same_node == true && shufflednodes.size() > 1) {
						dont_self_connect = common::Maths::getRandomBool(Cluster::SELF_CONNECTED_NODES_FRACTION);
#ifdef CLUSTER_DEBUG
						std::cout << "Cluster::updateConnectivity: " << nodes_count
						<< " same node, dont self connect  " << dont_self_connect << std::endl;
#endif

					}

					if (same_node == true && dont_self_connect == false) {
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);
#ifdef CLUSTER_DEBUG
						std::cout << "Cluster::updateConnectivity: " << nodes_count << " self connect  " << std::endl;
#endif
					} else if (same_node == true && dont_self_connect == true) {
						// add connection
						++it_shufflednodes;
						if (it_shufflednodes == it_shufflednodes_end) {
							it_shufflednodes = shufflednodes.begin();
						}
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);

					} else {
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					}
					++it_shufflednodes;
#ifdef CLUSTER_DEBUG
					++nodes_count;
					++shuffled_count;
#endif

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
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					} else if (same_node == true && dont_self_connect == true) {
						// add connection
						++it_shufflednodes;
						if (it_shufflednodes == it_shufflednodes_end) {
							it_shufflednodes = shufflednodes.begin();
						}
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);

					} else {
						clusterArchitect->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					}
					++it_shufflednodes;
				}

			}
			++it_allnodes;
		}
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
const components::ConnectionMap & Cluster::getConnectionMap() const{
	return connections;
}
components::ConnectionMap & Cluster::getMutableConnectionMap() {
	return connections;
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

void Cluster::enableDebug(bool b) {
	this->setDebug(b);
	nodes.enableDebug(b);
	connections.enableDebug(b);

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
	os << "Cluster: " << obj.getUUIDString() << "nodes:" << obj.getNodes().size() << " connections:"
			<< obj.getConnections().size() << std::endl;
	os << obj.getNodeMap() << std::endl;
	return os;
}

} //NAMESPACE

} //NAMESPACE
