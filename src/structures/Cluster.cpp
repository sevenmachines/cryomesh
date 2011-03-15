/*
 * Cluster.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#include "Cluster.h"
#include <list>
#include <algorithm>
#include "Fibre.h"

namespace cryomesh {

namespace structures {

Cluster::Cluster() {
}

Cluster::Cluster(int nodeCount, int connectivity) {
	this->createNodes(nodeCount);
	this->createConnectivity(connectivity);
}

Cluster::~Cluster() {
}

void Cluster::update() {
#ifdef CLUSTER_DEBUG
	std::cout<<"Cluster::update: "<<this<<std::endl;
#endif
	// update nodes
	nodes.update();
	// update connections
	connections.update();
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
					this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
					++it_shufflednodes;
				}

			} else if (asValue == AsMinumum) {
				for (int i = it_allnodes->second->getConnector().getOutputs().size(); i < connectivity; i++) {
					// add connection
					if (it_shufflednodes == it_shufflednodes_end) {
						it_shufflednodes = shufflednodes.begin();
					}
					this->createConnection(it_allnodes->second, *it_shufflednodes, 1);
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

std::ostream& operator<<(std::ostream & os, const Cluster & obj) {
	os << "Cluster: " << "nodes:" << obj.getNodes().size() << " connections:" << obj.getConnections().size()
			<< std::endl;
	os << obj.getNodeMap() << std::endl;
	return os;
}

}//NAMESPACE

}//NAMESPACE
