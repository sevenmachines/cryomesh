/*
 * ClusterArchitect.h
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#ifndef CLUSTERARCHITECT_H_
#define CLUSTERARCHITECT_H_

#include "ClusterAnalysisData.h"
#include "structures/Cluster.h"

#include <list>

namespace cryomesh {
namespace manipulators {

class ClusterArchitect {
public:

	ClusterArchitect(structures::Cluster & clus);
	virtual ~ClusterArchitect();
	virtual void runAnalysis();

	/**
	 * Get the history of analysis
	 *
	 * @return	const common::SimpleCollection<ClusterAnalysisData> &
	 * 	The container with the history of analysis
	 */
	virtual const std::list<ClusterAnalysisData> & getHistory() const ;

protected:

	structures::Cluster & cluster;

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
	 *
	 * @return std::list<ClusterAnalysisData> &
	 * 	List of all the analysis data history
	 */
	virtual std::list<ClusterAnalysisData>  getRandomNodes(int count);
	int getMaxHistorySize()const;
	void setMaxHistorySize(int sz);

private:
	std::list<ClusterAnalysisData> history;

	ClusterAnalysisData currentClusterAnalysisData;
	ClusterAnalysisData minClusterAnalysisData;
	ClusterAnalysisData maxClusterAnalysisData;
	ClusterAnalysisData averageClusterAnalysisData;

	int maxHistorySize;

	static const int DEFAULT_MAX_HISTORY_SIZE;
	void addHistoryEntry(	ClusterAnalysisData entry);
	void getHistoryStatistics(ClusterAnalysisData & minCad, ClusterAnalysisData & maxCad, ClusterAnalysisData & avCad);

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERARCHITECT_H_ */
