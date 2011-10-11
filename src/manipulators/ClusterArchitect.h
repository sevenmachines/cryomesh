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

	enum ConnectionStrategy{
		ENABLE_SELF_CONNECT=1, ENABLE_EVEN_DISTRIBUTION=2
	};

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

	void createConnection(boost::shared_ptr<components::Node> nodeStart,
			boost::shared_ptr<components::Node> nodeEnd, int connectivity =1) ;

	boost::shared_ptr< components::Connection > deleteConnection( boost::shared_ptr< components::Connection >  conn );

	/**
	 * Run an analysis on the cluster to decide what action to take on nodes and connections
	 */
	virtual const ClusterAnalysisData analyseCluster();

	/**
	 * Create a number of random nodes
	 */
	virtual std::list<boost::shared_ptr<cryomesh::components::Node> >  createRandomNodes(int count, int connectivity = 0, int strategy = 0);

	/**
	 * Create a number of random connections
	 */
	virtual std::list <boost::shared_ptr<components::Connection> > createRandomConnections(int count);

	/**
	 * Destroy random nodes
	 */
	virtual std::list <boost::shared_ptr<components::Node> > destroyRandomNodes(int count);

	/**
	 * Destroy random connections
	 */
	virtual std::list <boost::shared_ptr<components::Connection> > destroyRandomConnections(int count);

	/**
	 * Get a collection of random nodes from the cluster
	 *
	 * @param const int
	 * 	The number of random nodes to return
	 * @param const bool
	 * 	Allow the random nodes to be attached primaries, default false
	 *
	 * @return std::list<boost::shared_ptr<components::Node> >
	 * 	List of random nodes
	 */
	virtual std::list<boost::shared_ptr<components::Node> > getRandomNodes(const int count, const bool allow_primary ) ;


	/**
	 * Get a collection of random connections from the cluster
	 *
	 * @param const int
	 * 	The number of random connections to return
	 * @param const bool
	 * 	Allow the random connections to be attached primaries, default false
	 *
	 * @return std::list<boost::shared_ptr<components::Connection> >
	 * 	List of random connections
	 */
	virtual std::list<boost::shared_ptr<components::Connection> > getRandomConnections(const int count, const bool allow_primary ) ;

	int getMaxHistorySize() const;
	void setMaxHistorySize(int sz);


protected:
	structures::Cluster & cluster;
private:
	std::list<ClusterAnalysisData> history;

	ClusterAnalysisData currentClusterAnalysisData;
	ClusterAnalysisData minClusterAnalysisData;
	ClusterAnalysisData maxClusterAnalysisData;
	ClusterAnalysisData averageClusterAnalysisData;

	int maxHistorySize;

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

	static const int DEFAULT_MAX_HISTORY_SIZE;
	static const double DEFAULT_CONNECTIVITY_FRACTION;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERARCHITECT_H_ */
