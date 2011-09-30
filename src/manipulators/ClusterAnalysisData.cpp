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
		clusterEnergy(0), nodesToDestroy(0), connectionsToDestroy(0), nodesToCreate(0), connectionsToCreate(0), cycle(0) {
}

ClusterAnalysisData::ClusterAnalysisData(double cluster_energy, int nodes_destroy, int cons_destroy, int nodes_create,
		int cons_create) :
		clusterEnergy(cluster_energy), nodesToDestroy(nodes_destroy), connectionsToDestroy(cons_destroy), nodesToCreate(
				nodes_create), connectionsToCreate(cons_create), cycle(common::TimeKeeper::getTimeKeeper().getCycle()) {
}

ClusterAnalysisData::ClusterAnalysisData(const ClusterAnalysisData & obj) :
		Tagged(), clusterEnergy(obj.getClusterEnergy()), nodesToDestroy(obj.getNodesToDestroy()), connectionsToDestroy(
				obj.getConnectionsToDestroy()), nodesToCreate(obj.getNodesToCreate()), connectionsToCreate(
				obj.getConnectionsToCreate()), cycle(obj.getCycle()) {
}
ClusterAnalysisData::~ClusterAnalysisData() {
}

const ClusterAnalysisData ClusterAnalysisData::operator+(const ClusterAnalysisData & obj) const {
	ClusterAnalysisData temp_obj(*this);
	temp_obj += obj;
	return temp_obj;
}

ClusterAnalysisData & ClusterAnalysisData::operator+=(const ClusterAnalysisData & obj) {
	this->clusterEnergy += obj.getClusterEnergy();
	this->nodesToDestroy += obj.getNodesToDestroy();
	this->connectionsToDestroy += obj.getConnectionsToDestroy();
	this->nodesToCreate += obj.getNodesToCreate();
	this->connectionsToCreate += obj.getConnectionsToCreate();
	this->cycle += obj.getCycle();
	return *this;
}

const ClusterAnalysisData ClusterAnalysisData::operator/(const double dbl) const {
	ClusterAnalysisData temp_cad(*this);
	temp_cad /= dbl;
	return temp_cad;
}

ClusterAnalysisData & ClusterAnalysisData::operator/=(const double dbl) {
	if (dbl > 0) {
		this->clusterEnergy /= dbl;
		this->nodesToDestroy = static_cast<int>(ceil(static_cast<double>(this->nodesToDestroy) / dbl));
		this->connectionsToDestroy = static_cast<int>(ceil(static_cast<double>(this->connectionsToDestroy) / dbl));
		this->nodesToCreate = static_cast<int>(ceil(static_cast<double>(this->nodesToCreate) / dbl));
		this->connectionsToCreate = static_cast<int>(ceil(static_cast<double>(this->connectionsToCreate) / dbl));
		this->cycle = common::Cycle(static_cast<int>(static_cast<double>(this->cycle.toULInt()) / dbl));

	}
	return (*this);
}

bool ClusterAnalysisData::operator<(const ClusterAnalysisData & obj) const {
	return this->clusterEnergy > obj.getClusterEnergy();
}
bool ClusterAnalysisData::operator>(const ClusterAnalysisData & obj) const {
	return this->clusterEnergy < obj.getClusterEnergy();
}

double ClusterAnalysisData::getClusterEnergy() const {
	return clusterEnergy;
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

common::Cycle ClusterAnalysisData::getCycle() const {
	return cycle;
}
} /* namespace manipulators */
} /* namespace cryomesh */
