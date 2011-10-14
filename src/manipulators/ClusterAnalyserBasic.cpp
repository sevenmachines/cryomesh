/*
 * ClusterAnalyserBasic.cpp
 *
 *  Created on: 12 Oct 2011
 *      Author: niall
 */

#define CLUSTERANALYSERBASIC_DEBUG

#include "ClusterAnalyserBasic.h"

namespace cryomesh {
namespace manipulators {

ClusterAnalyserBasic::ClusterAnalyserBasic() {
	// TODO Auto-generated constructor stub

}

ClusterAnalyserBasic::~ClusterAnalyserBasic() {
	// TODO Auto-generated destructor stub
}

ClusterAnalysisData ClusterAnalyserBasic::analyseCluster(const structures::Cluster & cluster,
		const std::map<int, std::list<ClusterAnalysisData> > & histories)  const{

	const int cluster_node_count = cluster.getNodeMap().getSize();
	const int cluster_conn_count = cluster.getConnectionMap().getSize();

	// the current health
	double cluster_energy = cluster.getEnergy();
	// current cycle
	common::Cycle cycle = common::TimeKeeper::getTimeKeeper().getCycle();

	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_histories = histories.begin();
	const std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_histories_end = histories.end();
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_current_history = it_histories;
	++it_histories;
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_short_range_history = it_histories;
	++it_histories;
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_medium_range_history = it_histories;
	++it_histories;
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_long_range_history = it_histories;

	assert(it_current_history != it_histories_end);

	double node_creation_weight = 0;
	double node_destruction_weight = 0;
	double conn_creation_weight = 0;
	double conn_destruction_weight = 0;

	int node_create = 0;
	int nodes_destroy = 0;

	int conn_create = 0;
	int conn_destroy = 0;

	//progressive calculation of creation/destruction weights depending on history availability

	if (it_short_range_history != it_histories_end) {
		ClusterAnalysisData::RangeEnergy current_history_average_energy =
				it_short_range_history->second.rbegin()->getClusterRangeEnergy();
		// if short term trend is large + then
		// if short term trend is small + then
		// if short term trend is small - then
		// if short term trend is large + then

		if (it_medium_range_history != it_histories_end) {
			// if medium term trend is large + then
			// if medium term trend is small + then
			// if medium term trend is small - then
			// if medium term trend is large + then

			if (it_long_range_history != it_histories_end) {
				// if long term trend is large + then
				// if long term trend is small + then
				// if long term trend is small - then
				// if long term trend is large + then
			} // it_long_range_history
		} // it_medium_range_history
	} // it_short_range_history

	// all create/destroy weights should be clamped [0, 1]
	assert((node_creation_weight >= 0) && (node_creation_weight <= 1));
	assert((node_destruction_weight >= 0) && (node_destruction_weight <= 1));
	assert((conn_creation_weight >= 0) && (conn_creation_weight <= 1));
	assert((conn_destruction_weight >= 0) && (conn_destruction_weight <= 1));

	// finally calculate what restructuring to do
	node_create = node_creation_weight * cluster_node_count;
	nodes_destroy = node_destruction_weight * cluster_node_count;
	conn_create = conn_creation_weight * cluster_conn_count;
	conn_destroy = conn_destruction_weight * cluster_conn_count;

#ifdef CLUSTERANALYSERBASIC_DEBUG
	if (it_short_range_history == it_histories_end) {
		// check we did nothing
		assert(node_create == 0);
		assert(nodes_destroy == 0);
		assert(conn_create == 0);
		assert(conn_destroy == 0);
	}
#endif
	return ClusterAnalysisData(cluster_energy, node_creation_weight, node_destruction_weight, conn_creation_weight,
			conn_destruction_weight, node_create, nodes_destroy, conn_create, conn_destroy);
}

ClusterAnalysisData ClusterAnalyserBasic::calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const{
	// get averages of these
	ClusterAnalysisData::RangeEnergy cluster_energy;
	double node_creation_weight = 0;
	double node_destruction_weight = 0;
	double conn_creation_weight = 0;
	double conn_destruction_weight = 0;

	// get totals of these
	int node_create = 0;
	int nodes_destroy = 0;
	int conn_create = 0;
	int conn_destroy = 0;

	// forall in history
	{
		int count = 0;
		std::list<ClusterAnalysisData>::const_iterator it_history = history.begin();
		const std::list<ClusterAnalysisData>::const_iterator it_history_end = history.end();
		while (it_history != it_history_end) {
			const ClusterAnalysisData & ad = *it_history;

			cluster_energy += ad.getClusterRangeEnergy();

			node_creation_weight += ad.getNodeCreationWeight();
			node_destruction_weight += ad.getNodeDestructionWeight();
			conn_creation_weight += ad.getConnectionCreationWeight();
			conn_destruction_weight += ad.getConnectionDestructionWeight();

			node_create += ad.getNodesToCreate();
			nodes_destroy += ad.getNodesToDestroy();
			conn_create += ad.getConnectionsToCreate();
			conn_destroy += ad.getConnectionsToDestroy();

			++count;
			++it_history;
		}
		cluster_energy = cluster_energy / static_cast<double>(count);
		node_creation_weight = node_creation_weight / static_cast<double>(count);
		node_destruction_weight = node_destruction_weight / static_cast<double>(count);
		conn_creation_weight = conn_creation_weight / static_cast<double>(count);
		conn_destruction_weight = conn_destruction_weight / static_cast<double>(count);
	}
	return ClusterAnalysisData(cluster_energy, node_creation_weight, node_destruction_weight, conn_creation_weight,
			conn_destruction_weight, node_create, nodes_destroy, conn_create, conn_destroy);
}

} /* namespace manipulators */
} /* namespace cryomesh */
