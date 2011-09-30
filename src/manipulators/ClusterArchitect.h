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
#include "common/Cycle.h"
#include <map>
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
	virtual const std::list<ClusterAnalysisData> & getHistory() const;

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
	virtual std::vector<boost::shared_ptr<components::Node> > getRandomNodes(int count);
	int getMaxHistorySize() const;
	void setMaxHistorySize(int sz);

private:
	std::list<ClusterAnalysisData> history;

	ClusterAnalysisData currentClusterAnalysisData;
	ClusterAnalysisData minClusterAnalysisData;
	ClusterAnalysisData maxClusterAnalysisData;
	ClusterAnalysisData averageClusterAnalysisData;

	int maxHistorySize;

	static const int DEFAULT_MAX_HISTORY_SIZE;
	void addHistoryEntry(ClusterAnalysisData entry);
	void getHistoryStatistics(ClusterAnalysisData & minCad, ClusterAnalysisData & maxCad, ClusterAnalysisData & avCad);

	void splitHistoryByValue(double db, int countback, std::map<common::Cycle, ClusterAnalysisData> & below,
			std::map<common::Cycle, ClusterAnalysisData> & above) const;


	/**
	 * Get the fraction of history entries from the past count entries
	 * that are inside of a range, default is to check all of them
	 *
	 * @param double
	 * 	Value for entries to be above
	 * @param double
	 * 	Value for entries to be below
	 * @param int
	 * 	Number of past entries to go back
	 *
	 * @return std::vector<ClusterAnalysisData>
	 * 	Entries within range
	 */
	std::vector<ClusterAnalysisData> getHistoryEntriesInRange(double min_db, double max_db, int countback = 0) const;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERARCHITECT_H_ */
