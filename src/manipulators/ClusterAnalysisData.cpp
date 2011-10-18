/*
 * ClusterAnalysisData.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#include "ClusterAnalysisData.h"

namespace cryomesh {
namespace manipulators {

ClusterAnalysisData::ClusterAnalysisData() :
		clusterRangeEnergy(), nodeCreationWeight(0), nodeDestructionWeight(0), connectionCreationWeight(0), connectionDestructionWeight(
				0), nodesToCreate(0), nodesToDestroy(0), connectionsToCreate(0), connectionsToDestroy(0){
}
ClusterAnalysisData::ClusterAnalysisData(RangeEnergy cre) :
		clusterRangeEnergy(cre), nodeCreationWeight(0), nodeDestructionWeight(0), connectionCreationWeight(0), connectionDestructionWeight(
				0), nodesToCreate(0), nodesToDestroy(0), connectionsToCreate(0), connectionsToDestroy(0){
}

ClusterAnalysisData::ClusterAnalysisData(RangeEnergy range_energy, double node_creation_weight,
		double node_destruction_weight, double conn_creation_weight, double conn_destruction_weight, int node_create,
		int nodes_destroy, int conn_create, int conn_destroy) :
		clusterRangeEnergy(range_energy), nodeCreationWeight(node_creation_weight), nodeDestructionWeight(
				node_destruction_weight), connectionCreationWeight(conn_creation_weight), connectionDestructionWeight(
				conn_destruction_weight), nodesToCreate(node_create), nodesToDestroy(nodes_destroy), connectionsToCreate(
				conn_create), connectionsToDestroy(conn_destroy) {
}

ClusterAnalysisData::ClusterAnalysisData(const ClusterAnalysisData & obj) :
		Tagged(), clusterRangeEnergy(obj.getClusterRangeEnergy()), nodeCreationWeight(obj.getNodeCreationWeight()), nodeDestructionWeight(
				obj.getNodeDestructionWeight()), connectionCreationWeight(obj.getConnectionCreationWeight()), connectionDestructionWeight(
				obj.getConnectionDestructionWeight()), nodesToCreate(obj.getNodesToCreate()), nodesToDestroy(
				obj.getNodesToDestroy()), connectionsToCreate(obj.getConnectionsToCreate()), connectionsToDestroy(
				obj.getConnectionsToDestroy()) {
}
ClusterAnalysisData::~ClusterAnalysisData() {
}

ClusterAnalysisData::RangeEnergy ClusterAnalysisData::getClusterRangeEnergy() const {
	return clusterRangeEnergy;
}
double ClusterAnalysisData::getNodeCreationWeight() const {
	return nodeCreationWeight;
}
double ClusterAnalysisData::getNodeDestructionWeight() const {
	return nodeDestructionWeight;
}
double ClusterAnalysisData::getConnectionCreationWeight() const {
	return connectionCreationWeight;
}
double ClusterAnalysisData::getConnectionDestructionWeight() const {
	return connectionDestructionWeight;
}

int ClusterAnalysisData::getNodesToDestroy() const {
	return nodesToDestroy;
}
int ClusterAnalysisData::getConnectionsToDestroy() const {
	return connectionsToDestroy;
}
int ClusterAnalysisData::getNodesToCreate() const {
	return nodesToCreate;
}
int ClusterAnalysisData::getConnectionsToCreate() const {
	return connectionsToCreate;
}

void ClusterAnalysisData::setClusterRangeEnergy(RangeEnergy range_energy) {
	this->clusterRangeEnergy = range_energy;
}

void ClusterAnalysisData::setConnectionCreationWeight(double connectionCreationWeight) {
	this->connectionCreationWeight = connectionCreationWeight;
}

void ClusterAnalysisData::setConnectionDestructionWeight(double connectionDestructionWeight) {
	this->connectionDestructionWeight = connectionDestructionWeight;
}

void ClusterAnalysisData::setConnectionsToCreate(int connectionsToCreate) {
	this->connectionsToCreate = connectionsToCreate;
}

void ClusterAnalysisData::setConnectionsToDestroy(int connectionsToDestroy) {
	this->connectionsToDestroy = connectionsToDestroy;
}

void ClusterAnalysisData::setNodeCreationWeight(double nodeCreationWeight) {
	this->nodeCreationWeight = nodeCreationWeight;
}

void ClusterAnalysisData::setNodeDestructionWeight(double nodeDestructionWeight) {
	this->nodeDestructionWeight = nodeDestructionWeight;
}

void ClusterAnalysisData::setNodesToCreate(int nodesToCreate) {
	this->nodesToCreate = nodesToCreate;
}

void ClusterAnalysisData::setNodesToDestroy(int nodesToDestroy) {
	this->nodesToDestroy = nodesToDestroy;
}

} /* namespace manipulators */
}
/* namespace cryomesh */
