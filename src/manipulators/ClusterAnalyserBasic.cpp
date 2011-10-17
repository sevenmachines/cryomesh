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
}

ClusterAnalyserBasic::~ClusterAnalyserBasic() {
}

ClusterAnalysisData ClusterAnalyserBasic::analyseCluster(const structures::Cluster & cluster,
		const std::map<int, std::list<ClusterAnalysisData> > & histories) const {

	const int current_cluster_node_count = cluster.getNodeMap().getSize();
	const int current_cluster_conn_count = cluster.getConnectionMap().getSize();

	// current cycle
	ClusterAnalysisData::RangeEnergy current_range_energy(cluster.getEnergy());
#ifdef CLUSTERANALYSERBASIC_DEBUG
	assert(current_range_energy.energy >= -1);
	assert(current_range_energy.energy <= 1);
#endif
	const std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_histories_end = histories.end();
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_histories = histories.begin();

	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_short_range_history = it_histories_end;
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_medium_range_history = it_histories_end;
	std::map<int, std::list<ClusterAnalysisData> >::const_iterator it_long_range_history = it_histories_end;

	if (it_histories != it_histories_end) {
		it_short_range_history = it_histories;
		++it_histories;
		if (it_histories != it_histories_end) {

			it_medium_range_history = it_histories;
			++it_histories;
			if (it_histories != it_histories_end) {
				it_long_range_history = it_histories;
			}
		}
	}

	double node_creation_weight = 0;
	double node_destruction_weight = 0;
	double conn_creation_weight = 0;
	double conn_destruction_weight = 0;

	//progressive calculation of creation/destruction weights depending on history availability

	if (it_short_range_history != it_histories_end) {
		ClusterAnalysisData cad_short_summary = this->calculateRangeEnergies(it_short_range_history->second);

		if (it_medium_range_history != it_histories_end) {

			const double medium_base_fraction = 0.01;

			ClusterAnalysisData cad_medium_summary = this->calculateRangeEnergies(it_medium_range_history->second);
			double val_diff_medium_summary = cad_short_summary.getClusterRangeEnergy().energy
					- cad_medium_summary.getClusterRangeEnergy().energy;
			//	double min_diff_medium_summary = cad_short_summary.getClusterRangeEnergy().energy
			//		- cad_medium_summary.getClusterRangeEnergy().energyMin;
			//double max_diff_medium_summary = cad_short_summary.getClusterRangeEnergy().energy
			//		- cad_medium_summary.getClusterRangeEnergy().energyMax;

			EnergyVariation medium_variation = this->getFuzzyEnergyVariation(val_diff_medium_summary, 1.0, 0.01);
			if (medium_variation <= EnergyVariation::HIGH_NEGATIVE) {
				// kill/create a lot of nodes
				node_destruction_weight += medium_base_fraction;
				node_creation_weight += medium_base_fraction;
			} else if (medium_variation == EnergyVariation::MEDIUM_NEGATIVE) {
				// kill/create some nodes
				node_destruction_weight += medium_base_fraction / 2.0;
				node_creation_weight += medium_base_fraction / 2.0;
			} else if (medium_variation == EnergyVariation::SMALL_NEGATIVE) {
				// kill/create some connections
				conn_destruction_weight += medium_base_fraction;
				conn_creation_weight += medium_base_fraction;
			} else if (medium_variation == EnergyVariation::STAGNANT_NEGATIVE) {
				// kill/create a few connections
				conn_destruction_weight += medium_base_fraction / 2.0;
				conn_creation_weight += medium_base_fraction / 2.0;
			} else if ((medium_variation == EnergyVariation::ZERO)
					|| (medium_variation == EnergyVariation::STAGNANT_POSITIVE)) {
				//create a few nodes
				node_creation_weight += medium_base_fraction / 2.0;
			} else if (medium_variation == EnergyVariation::SMALL_POSITIVE) {
				// create a few connections
				conn_creation_weight += medium_base_fraction / 2.0;
			} else if (medium_variation == EnergyVariation::MEDIUM_POSITIVE) {
				// create a very few connections
				conn_creation_weight += medium_base_fraction / 4.0;
			} else if (medium_variation >= EnergyVariation::HIGH_POSITIVE) {
				// everything good, do nothing
			}

			if (it_long_range_history != it_histories_end) {
				ClusterAnalysisData cad_long_summary = this->calculateRangeEnergies(it_long_range_history->second);
				double val_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
						- cad_long_summary.getClusterRangeEnergy().energy;
				//	double min_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
				//			- cad_long_summary.getClusterRangeEnergy().energyMin;
				//	double max_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
				//			- cad_long_summary.getClusterRangeEnergy().energyMax;

				const double long_base_fraction = 0.1;
				EnergyVariation long_variation = this->getFuzzyEnergyVariation(val_diff_long_summary, 1.0, 0.01);
				if (long_variation <= EnergyVariation::HIGH_NEGATIVE) {
					// kill/create a lot of nodes
					node_destruction_weight += long_base_fraction;
					node_creation_weight += long_base_fraction;
				} else if (long_variation == EnergyVariation::MEDIUM_NEGATIVE) {
					// kill/create some nodes
					node_destruction_weight += long_base_fraction / 2.0;
					node_creation_weight += long_base_fraction / 2.0;
				} else if (long_variation == EnergyVariation::SMALL_NEGATIVE) {
					// kill/create some connections
					conn_destruction_weight += long_base_fraction;
					conn_creation_weight += long_base_fraction;
				} else if (long_variation == EnergyVariation::STAGNANT_NEGATIVE) {
					// kill/create a few connections
					conn_destruction_weight += long_base_fraction / 2.0;
					conn_creation_weight += long_base_fraction / 2.0;
				} else if ((long_variation == EnergyVariation::ZERO)
						|| (long_variation == EnergyVariation::STAGNANT_POSITIVE)) {
					//create a few nodes
					node_creation_weight += long_base_fraction / 2.0;
				} else if (long_variation == EnergyVariation::SMALL_POSITIVE) {
					// create a few connections
					conn_creation_weight += long_base_fraction / 2.0;
				} else if (long_variation == EnergyVariation::MEDIUM_POSITIVE) {
					// create a very few connections
					conn_creation_weight += long_base_fraction / 4.0;
				} else if (long_variation >= EnergyVariation::HIGH_POSITIVE) {
					// everything good, do nothing
				}
			} // it_long_range_history
		} // it_medium_range_history
	} // it_short_range_history

	// all create/destroy weights should be clamped [0, 1]
	assert((node_creation_weight >= 0) && (node_creation_weight <= 1));
	assert((node_destruction_weight >= 0) && (node_destruction_weight <= 1));
	assert((conn_creation_weight >= 0) && (conn_creation_weight <= 1));
	assert((conn_destruction_weight >= 0) && (conn_destruction_weight <= 1));

	// finally calculate what restructuring to do
	int node_create = node_creation_weight * current_cluster_node_count;
	int nodes_destroy = node_destruction_weight * current_cluster_node_count;

	int conn_create = conn_creation_weight * current_cluster_conn_count;
	int conn_destroy = conn_destruction_weight * current_cluster_conn_count;

	// force some effect when we get fractions less than 1
	if (node_creation_weight > 0) {
		node_create = std::max(node_create, 1);
	}
	if (node_destruction_weight > 0) {
		nodes_destroy = std::max(nodes_destroy, 1);
	}
	if (conn_creation_weight > 0) {
		conn_create = std::max(conn_create, 1);
	}
	if (conn_destruction_weight > 0) {
		conn_destroy = std::max(conn_destroy, 1);
	}

	if (it_short_range_history == it_histories_end) {
		// check we did nothing
		assert(node_create == 0);
		assert(nodes_destroy == 0);
		assert(conn_create == 0);
		assert(conn_destroy == 0);
	}
	return ClusterAnalysisData(current_range_energy, node_creation_weight, node_destruction_weight,
			conn_creation_weight, conn_destruction_weight, node_create, nodes_destroy, conn_create, conn_destroy);
}

ClusterAnalysisData ClusterAnalyserBasic::calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const {
	// get averages of these
	ClusterAnalysisData::RangeEnergy cluster_range_energy;
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
		if (it_history != it_history_end) {
			const ClusterAnalysisData & ad = *it_history;
			cluster_range_energy = ad.getClusterRangeEnergy();
			node_creation_weight = ad.getNodeCreationWeight();
			node_destruction_weight = ad.getNodeDestructionWeight();
			conn_creation_weight = ad.getConnectionCreationWeight();
			conn_destruction_weight = ad.getConnectionDestructionWeight();
			node_create = ad.getNodesToCreate();
			nodes_destroy = ad.getNodesToDestroy();
			conn_create = ad.getConnectionsToCreate();
			conn_destroy = ad.getConnectionsToDestroy();
		}
		++it_history;
		++count;
		while (it_history != it_history_end) {
			const ClusterAnalysisData & ad = *it_history;
			cluster_range_energy += ad.getClusterRangeEnergy();
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
		cluster_range_energy = cluster_range_energy / static_cast<double>(count);
		node_creation_weight = node_creation_weight / static_cast<double>(count);
		node_destruction_weight = node_destruction_weight / static_cast<double>(count);
		conn_creation_weight = conn_creation_weight / static_cast<double>(count);
		conn_destruction_weight = conn_destruction_weight / static_cast<double>(count);
	}
	return ClusterAnalysisData(cluster_range_energy, node_creation_weight, node_destruction_weight,
			conn_creation_weight, conn_destruction_weight, node_create, nodes_destroy, conn_create, conn_destroy);
}

ClusterAnalyserBasic::EnergyVariation ClusterAnalyserBasic::getEnergyVariation(double d, double range) const {
	EnergyVariation varation;
	if (d > range) {
		varation = OUT_OF_RANGE_POSITIVE;
	} else if (d < -range) {
		varation = OUT_OF_RANGE_NEGATIVE;
	} else if (d >= 0.8 * range) {
		varation = HIGH_POSITIVE;
	} else if (d >= 0.5 * range) {
		varation = MEDIUM_POSITIVE;

	} else if (d >= 0.1 * range) {
		varation = SMALL_POSITIVE;
	} else if (d > 0 * range) {
		varation = STAGNANT_POSITIVE;
	} else if (d == 0) {
		varation = ZERO;

	} else if (d <= -0.8 * range) {
		varation = HIGH_NEGATIVE;
	} else if (d <= -0.5 * range) {
		varation = MEDIUM_NEGATIVE;
	} else if (d <= -0.1 * range) {
		varation = SMALL_NEGATIVE;
	} else if (d < 0) {
		varation = STAGNANT_NEGATIVE;
	} else {
		// should never be here
		assert(false);
	}
	return varation;
}
ClusterAnalyserBasic::EnergyVariation ClusterAnalyserBasic::getFuzzyEnergyVariation(double d, double range,
		double fuzz_factor) const {
	const double fuzzy_factor = common::Maths::getRandomDouble(-fuzz_factor * range, fuzz_factor * range);
	double fuzzy_var = d + fuzzy_factor;
	return (this->getEnergyVariation(fuzzy_var, range));
}
} /* namespace manipulators */
} /* namespace cryomesh */
