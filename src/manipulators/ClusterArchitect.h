/*
 * ClusterArchitect.h
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#ifndef CLUSTERARCHITECT_H_
#define CLUSTERARCHITECT_H_

#include "structures/Cluster.h"

namespace cryomesh {
namespace manipulators {

class ClusterArchitect {
public:
	struct ClusterAnalysisData {
		ClusterAnalysisData(int nodes_destroy, int cons_destroy, int nodes_create, int cons_create) :
				nodesToDestroy(nodes_destroy), connectionsToDestroy(cons_destroy), nodesToCreate(nodes_create), connectionsToCreate(
						cons_create) {
		}
		int nodesToDestroy;
		int connectionsToDestroy;
		int nodesToCreate;
		int connectionsToCreate;
	};

	ClusterArchitect(structures::Cluster & clus);
	virtual ~ClusterArchitect();
	virtual void runAnalysis();

protected:

	structures::Cluster  & cluster;

	/**
	 * Run an analysis on the cluster to decide what action to take on nodes and connections
	 */
	virtual const ClusterAnalysisData analyseCluster();

	/**
	 * Create a number of random nodes
	 */
	virtual void birthRandomNodes(int count);

	/**
	 * Create a number of random connections
	 */
	virtual void birthRandomConnections(int count);

	/**
	 * Destroy random nodes
	 */
	virtual void destroyRandomNodes(int count);

	/**
	 * Destroy random connections
	 */
	virtual void destroyRandomConnections(int count);

	/**
	 * Get a collection of random nodes from the cluster
	 */
	virtual std::vector<components::Node> getRandomNodes(int count);

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERARCHITECT_H_ */
