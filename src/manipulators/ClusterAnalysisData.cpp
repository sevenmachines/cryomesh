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
		clusterActivity(0), nodesToDestroy(0), connectionsToDestroy(0), nodesToCreate(0), connectionsToCreate(0) {
}

ClusterAnalysisData::ClusterAnalysisData(double cluster_act, int nodes_destroy, int cons_destroy, int nodes_create,
		int cons_create) :
		clusterActivity(cluster_act), nodesToDestroy(nodes_destroy), connectionsToDestroy(cons_destroy), nodesToCreate(
				nodes_create), connectionsToCreate(cons_create) {
}

ClusterAnalysisData::ClusterAnalysisData(const ClusterAnalysisData & obj) :
		Tagged() {
	this->clusterActivity = obj.getClusterActivity();
	this->nodesToDestroy = obj.getNodesToDestroy();
	this->connectionsToDestroy = obj.getConnectionsToDestroy();
	this->nodesToCreate = obj.getNodesToCreate();
	this->connectionsToCreate = obj.getConnectionsToCreate();
}
ClusterAnalysisData::~ClusterAnalysisData() {
}

const ClusterAnalysisData ClusterAnalysisData::operator+(const ClusterAnalysisData & obj) const {
	ClusterAnalysisData temp_obj(*this);
	temp_obj += obj;
	return temp_obj;
}

ClusterAnalysisData & ClusterAnalysisData::operator+=(const ClusterAnalysisData & obj) {
	this->clusterActivity += obj.getClusterActivity();
	this->nodesToDestroy += obj.getNodesToDestroy();
	this->connectionsToDestroy += obj.getConnectionsToDestroy();
	this->nodesToCreate += obj.getNodesToCreate();
	this->connectionsToCreate += obj.getConnectionsToCreate();
	return *this;
}

const ClusterAnalysisData ClusterAnalysisData::operator/(const double dbl) const {
	ClusterAnalysisData temp_cad(*this);
	temp_cad /= dbl;
	return temp_cad;
}

ClusterAnalysisData & ClusterAnalysisData::operator/=(const double dbl) {
	this->clusterActivity /= dbl;
	this->nodesToDestroy = static_cast<int>(ceil(static_cast<double>(this->nodesToDestroy) / dbl));
	this->connectionsToDestroy = static_cast<int>(ceil(static_cast<double>(this->connectionsToDestroy) / dbl));
	this->nodesToCreate = static_cast<int>(ceil(static_cast<double>(this->nodesToCreate) / dbl));
	this->connectionsToCreate = static_cast<int>(ceil(static_cast<double>(this->connectionsToCreate) / dbl));
	return (*this);
}

bool ClusterAnalysisData::operator<(const ClusterAnalysisData & obj) const {
	return this->clusterActivity > obj.getClusterActivity();
}
bool ClusterAnalysisData::operator>(const ClusterAnalysisData & obj) const {
	return this->clusterActivity < obj.getClusterActivity();
}

double ClusterAnalysisData::getClusterActivity() const {
	return clusterActivity;
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
} /* namespace manipulators */
} /* namespace cryomesh */
