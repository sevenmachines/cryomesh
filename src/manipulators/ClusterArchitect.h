/*
 * ClusterArchitect.h
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#ifndef CLUSTERARCHITECT_H_
#define CLUSTERARCHITECT_H_

#include "ClusterAnalysisData.h"
#include "IClusterAnalyser.h"

#include "structures/Cluster.h"
#include "common/Cycle.h"
#include <map>
#include <list>
#include <set>

namespace cryomesh {
namespace manipulators {

class ClusterArchitect {
public:

	enum ConnectionStrategy{
		ENABLE_SELF_CONNECT=1, ENABLE_EVEN_DISTRIBUTION=2
	};

	ClusterArchitect(structures::Cluster & clus, const  int max_history_sz = DEFAULT_MAX_HISTORY_SIZE, const  int history_stepping_factor = DEFAULT_HISTORY_STEPPING_FACTOR );
	virtual ~ClusterArchitect();
	virtual void runAnalysis();

	/**
	 * Get the history of analysis
	 *
	 * @return	const common::SimpleCollection<ClusterAnalysisData> &
	 * 	The container with the history of analysis
	 */
    const virtual std::map<int,std::list<ClusterAnalysisData> > & getHistories() const;
    void createConnection(boost::shared_ptr<components::Node> nodeStart, boost::shared_ptr<components::Node> nodeEnd, int connectivity = 1);
    boost::shared_ptr<components::Connection> deleteConnection(boost::shared_ptr<components::Connection> conn);
    /**
	 * Create a number of random nodes
	 */
    virtual std::set<boost::shared_ptr<cryomesh::components::Node> > createRandomNodes(int count, int connectivity = 0, int strategy = 0);
    /**
	 * Create a number of random connections
	 */
    virtual std::map<boost::uuids::uuid,boost::shared_ptr<components::Connection> > createRandomConnections(int count);
    /**
	 * Destroy random nodes
	 */
    virtual std::map<boost::uuids::uuid,boost::shared_ptr<components::Node> > destroyRandomNodes(int count);
    /**
	 * Destroy random connections
	 */
    virtual std::map<boost::uuids::uuid,boost::shared_ptr<components::Connection> > destroyRandomConnections(int count);
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
    virtual std::map<boost::uuids::uuid,boost::shared_ptr<components::Node> > getRandomNodes(const int count, const bool allow_primary);
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
    virtual std::map<boost::uuids::uuid,boost::shared_ptr<components::Connection> > getRandomConnections(const int count, const bool allow_primary);
    int getMaxHistorySize() const;
    void setMaxHistorySize(int sz);
    ClusterAnalysisData getCurrentClusterAnalysisData() const;
    const std::list<ClusterAnalysisData> & getCurrentHistory() const;
     int getHistorySteppingFactor() const;
    void setCurrentClusterAnalysisData(ClusterAnalysisData currentClusterAnalysisData);
    void setCurrentHistory(std::list<ClusterAnalysisData> currentHistory);

    std::ostream & printAllHistory(std::ostream & os);
    const structures::Cluster & getCluster() const;
    const boost::shared_ptr<IClusterAnalyser> getClusterAnalyser() const;
    void setClusterAnalyser(boost::shared_ptr<IClusterAnalyser> clusterAnalyser);
    void setHistories(std::map<int,std::list<ClusterAnalysisData> > histories);

protected:
	structures::Cluster & cluster;

	/**
	 * Default value multiple for history stepping
	 *
	 * @var const unsigned int
	 */
	static const unsigned int DEFAULT_HISTORY_STEPPING_FACTOR;
	static const int DEFAULT_MAX_HISTORY_SIZE;
	static const double DEFAULT_CONNECTIVITY_FRACTION;

private:
	/**
	 * The current active history set
	 *
	 * @var std::list<ClusterAnalysisData >
	 */
	std::list<ClusterAnalysisData > currentHistory;

	/**
	 * Map of all histories, the int represents the cycle seperation, the list is the
	 * resultant values/averages. eg
	 * - mapping of 1 to a list of ClusterAnalysisData is the standard save of every cycles history (up to cutoff)
	 * - mapping of 10 means that every 10 cycles are averaged and the result added to the mapped list
	 *
	 * Note that mapping steps are recursive, we can only have int keys as multiples of each other, eg
	 * {1, 2, 4, 8, etc} or {1, 10, 100, ... } , so { 1, a, a^2, a^3, ... }
	 *
	 */
	std::map<int, std::list<ClusterAnalysisData> > histories;
	const  int historySteppingFactor;

	boost::shared_ptr< IClusterAnalyser > clusterAnalyser;
	ClusterAnalysisData currentClusterAnalysisData;

	int maxHistorySize;

	void addHistoryEntry(ClusterAnalysisData entry);

	void splitHistoryByValue(const std::list<ClusterAnalysisData> & history, double db, int countback, std::map<common::Cycle, ClusterAnalysisData> & below,
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
	std::vector<ClusterAnalysisData> getHistoryEntriesInRange(const std::list<ClusterAnalysisData> & history, double min_db, double max_db, int countback = 0) const;


};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERARCHITECT_H_ */
