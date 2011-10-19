/*
 * ClusterAnalyserBasic.cpp
 *
 *  Created on: 12 Oct 2011
 *      Author: niall
 */

#define CLUSTERANALYSERBASIC_DEBUG

#include "ClusterAnalyserBasic.h"
#include "ClusterArchitect.h"

namespace cryomesh {
namespace manipulators {

ClusterAnalyserBasic::ClusterAnalyserBasic(const ClusterArchitect & ca) : IClusterAnalyser(),
		clusterArchitect(ca){
}

ClusterAnalyserBasic::~ClusterAnalyserBasic() {
}

ClusterAnalysisData ClusterAnalyserBasic::analyseCluster(const structures::Cluster & cluster,
		const std::map<int, std::list<ClusterAnalysisData> > & histories) {

	--nodeRestructuring;
	--connectionRestructuring;

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
	int node_create = 0;
	int nodes_destroy = 0;
	int conn_create = 0;
	int conn_destroy = 0;

	bool any_restructuring_enabled = nodeRestructuring.isAnyRestructuringEnabled()
			|| connectionRestructuring.isAnyRestructuringEnabled();
	if (any_restructuring_enabled == true) {
		//progressive calculation of creation/destruction weights depending on history availability

		if (it_short_range_history != it_histories_end) {

			//	bool any_short_restructure_enabled = nodeRestructuring.isAnyShortRestructuringEnabled()
			//		|| connectionRestructuring.isAnyShortRestructuringEnabled();
			bool any_medium_restructure_enabled = nodeRestructuring.isAnyMediumRestructuringEnabled()
					|| connectionRestructuring.isAnyMediumRestructuringEnabled();
			bool any_long_restructure_enabled = nodeRestructuring.isAnyLongRestructuringEnabled()
					|| connectionRestructuring.isAnyLongRestructuringEnabled();

			if (it_medium_range_history != it_histories_end
					&& (any_medium_restructure_enabled || any_long_restructure_enabled)) {

				const double medium_base_fraction = 0.01;
				ClusterAnalysisData cad_short_summary = this->calculateRangeEnergies(it_short_range_history->second);

				ClusterAnalysisData cad_medium_summary = this->calculateRangeEnergies(it_medium_range_history->second);
				double val_diff_medium_summary = cad_short_summary.getClusterRangeEnergy().energy
						- cad_medium_summary.getClusterRangeEnergy().energy;

				EnergyVariationWeightingMap medium_variation = this->getEnergyVariationMap(val_diff_medium_summary,
						1.0);

				if (any_medium_restructure_enabled == true) {
					if (medium_variation.variationMap[EnergyVariation::HIGH_NEGATIVE] > 0) {
						// kill/create a lot of nodes
						node_destruction_weight += medium_base_fraction
								* medium_variation.variationMap[EnergyVariation::HIGH_NEGATIVE];
						node_creation_weight += medium_base_fraction
								* medium_variation.variationMap[EnergyVariation::HIGH_NEGATIVE];
					}
					if (medium_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE] > 0) {
						// kill/create some nodes
						node_destruction_weight += medium_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE]
								* medium_base_fraction / 2.0;
						node_creation_weight += medium_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE]
								* medium_base_fraction / 2.0;
					}
					if (medium_variation.variationMap[EnergyVariation::SMALL_NEGATIVE] > 0) {
						// kill/create some connections
						conn_destruction_weight += medium_base_fraction
								* medium_variation.variationMap[EnergyVariation::SMALL_NEGATIVE];
						conn_creation_weight += medium_base_fraction
								* medium_variation.variationMap[EnergyVariation::SMALL_NEGATIVE];
					}
					if (medium_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE] > 0) {
						// kill/create a few connections
						conn_destruction_weight += medium_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE]
								* medium_base_fraction / 2.0;
						conn_creation_weight += medium_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE]
								* medium_base_fraction / 2.0;
					}
					if ((medium_variation.variationMap[EnergyVariation::STAGNANT_POSITIVE] > 0)) {
						//create a few nodes
						node_creation_weight += medium_variation.variationMap[EnergyVariation::STAGNANT_POSITIVE]
								* medium_base_fraction / 2.0;
					}
					if (medium_variation.variationMap[EnergyVariation::SMALL_POSITIVE] > 0) {
						// create a few connections
						conn_creation_weight += medium_variation.variationMap[EnergyVariation::SMALL_POSITIVE]
								* medium_base_fraction / 2.0;
					}
					if (medium_variation.variationMap[EnergyVariation::MEDIUM_POSITIVE] > 0) {
						// create a very few connections
						conn_creation_weight += medium_variation.variationMap[EnergyVariation::MEDIUM_POSITIVE]
								* medium_base_fraction / 4.0;
					}
					if (medium_variation.variationMap[EnergyVariation::HIGH_POSITIVE] > 0) {
						// everything good, do nothing
					}

					// now reset the countdown
					nodeRestructuring.setMediumCountdown(
							clusterArchitect.getMaxHistorySize() * clusterArchitect.getHistorySteppingFactor());
				}

				if (it_long_range_history != it_histories_end && (any_long_restructure_enabled)) {
					ClusterAnalysisData cad_long_summary = this->calculateRangeEnergies(it_long_range_history->second);
					double val_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
							- cad_long_summary.getClusterRangeEnergy().energy;
					//	double min_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
					//			- cad_long_summary.getClusterRangeEnergy().energyMin;
					//	double max_diff_long_summary = cad_medium_summary.getClusterRangeEnergy().energy
					//			- cad_long_summary.getClusterRangeEnergy().energyMax;

					const double long_base_fraction = 0.1;
					EnergyVariationWeightingMap long_variation = this->getEnergyVariationMap(val_diff_long_summary,
							1.0);
					if (long_variation.variationMap[EnergyVariation::HIGH_NEGATIVE] > 0) {
						// kill/create a lot of nodes
						node_destruction_weight += long_variation.variationMap[EnergyVariation::HIGH_NEGATIVE]
								* long_base_fraction;
						node_creation_weight += long_variation.variationMap[EnergyVariation::HIGH_NEGATIVE]
								* long_base_fraction;
					} else if (long_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE] > 0) {
						// kill/create some nodes
						node_destruction_weight += long_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE]
								* long_base_fraction / 2.0;
						node_creation_weight += long_variation.variationMap[EnergyVariation::MEDIUM_NEGATIVE]
								* long_base_fraction / 2.0;
					} else if (long_variation.variationMap[EnergyVariation::SMALL_NEGATIVE] > 0) {
						// kill/create some connections
						conn_destruction_weight += long_variation.variationMap[EnergyVariation::SMALL_NEGATIVE]
								* long_base_fraction;
						conn_creation_weight += long_variation.variationMap[EnergyVariation::SMALL_NEGATIVE]
								* long_base_fraction;
					} else if (long_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE] > 0) {
						// kill/create a few connections
						conn_destruction_weight += long_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE]
								* long_base_fraction / 2.0;
						conn_creation_weight += long_variation.variationMap[EnergyVariation::STAGNANT_NEGATIVE]
								* long_base_fraction / 2.0;
					} else if (long_variation.variationMap[EnergyVariation::STAGNANT_POSITIVE] > 0) {
						//create a few nodes
						node_creation_weight += long_variation.variationMap[EnergyVariation::STAGNANT_POSITIVE]
								* long_base_fraction / 2.0;
					} else if (long_variation.variationMap[EnergyVariation::SMALL_POSITIVE] > 0) {
						// create a few connections
						conn_creation_weight += long_variation.variationMap[EnergyVariation::SMALL_POSITIVE]
								* long_base_fraction / 2.0;
					} else if (long_variation.variationMap[EnergyVariation::MEDIUM_POSITIVE] > 0) {
						// create a very few connections
						conn_creation_weight += long_variation.variationMap[EnergyVariation::MEDIUM_POSITIVE]
								* long_base_fraction / 4.0;
					} else if (long_variation.variationMap[EnergyVariation::HIGH_POSITIVE] > 0) {
						// everything good, do nothing
					}
					// now reset the countdown
					nodeRestructuring.setLongCountdown(
							clusterArchitect.getMaxHistorySize() * clusterArchitect.getHistorySteppingFactor()
									* clusterArchitect.getHistorySteppingFactor());
				} // it_long_range_history
			} // it_medium_range_history
		} // it_short_range_history

		// all create/destroy weights should be clamped [0, 1]
		assert((node_creation_weight >= 0) && (node_creation_weight <= 1));
		assert((node_destruction_weight >= 0) && (node_destruction_weight <= 1));
		assert((conn_creation_weight >= 0) && (conn_creation_weight <= 1));
		assert((conn_destruction_weight >= 0) && (conn_destruction_weight <= 1));

		// finally calculate what restructuring to do
		node_create = node_creation_weight * current_cluster_node_count;
		nodes_destroy = node_destruction_weight * current_cluster_node_count;

		conn_create = conn_creation_weight * current_cluster_conn_count;
		conn_destroy = conn_destruction_weight * current_cluster_conn_count;

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

ClusterAnalyserBasic::EnergyVariationWeightingMap ClusterAnalyserBasic::getEnergyVariationMap(const double energy_input,
		double range) const {
	EnergyVariationWeightingMap evwp;
	const double DELTA = 0.000001;
	if (range < 0) {
		range = -range;
	}

	assert(range != 0);
	assert(range > DELTA);

	// map everything to [1,1]
	double normalised_energy = energy_input * (1.0 / range);

	// get middle category

	EnergyVariation middleVariation;
	EnergyVariation upperVariation;
	EnergyVariation lowerVariation;
	double midPoint = 0.0;
	//double highBorderPoint = 0;
	//double lowBorderPoint = 0;
	if (normalised_energy > 1) {
		middleVariation = OUT_OF_RANGE_POSITIVE;
	} else if (normalised_energy < -1) {
		middleVariation = OUT_OF_RANGE_NEGATIVE;
	} else if (normalised_energy >= 0.8) {
		middleVariation = HIGH_POSITIVE;
		lowerVariation = MEDIUM_POSITIVE;
		upperVariation = OUT_OF_RANGE_POSITIVE;

		midPoint = 0.9;
		//highBorderPoint = 1.0;
		//lowBorderPoint = 0.8;
	} else if (normalised_energy >= 0.5) {
		middleVariation = MEDIUM_POSITIVE;
		midPoint = 0.65;
		//highBorderPoint = 0.8;
		//lowBorderPoint = 0.5;
		lowerVariation = SMALL_POSITIVE;
		upperVariation = HIGH_POSITIVE;

	} else if (normalised_energy >= 0.1) {
		middleVariation = SMALL_POSITIVE;
		midPoint = 0.3;
		//highBorderPoint = 0.5;
		//lowBorderPoint = 0.1;
		lowerVariation = STAGNANT_POSITIVE;
		upperVariation = MEDIUM_POSITIVE;
	} else if (normalised_energy > 0 + DELTA) {
		middleVariation = STAGNANT_POSITIVE;
		midPoint = 0.05;
		//highBorderPoint = 0.1;
		//lowBorderPoint = 0;
		lowerVariation = ZERO;
		upperVariation = SMALL_POSITIVE;
	} else if ((normalised_energy < 0 + DELTA) && (normalised_energy > 0 - DELTA)) {
		middleVariation = STAGNANT_POSITIVE;
		midPoint = 0.05;
		//highBorderPoint = 0.1;
		//lowBorderPoint = 0;
		lowerVariation = ZERO;
		upperVariation = SMALL_POSITIVE;
	} else if (normalised_energy <= -0.8) {
		middleVariation = HIGH_NEGATIVE;
		midPoint = -0.9;
		//highBorderPoint = -0.8;
		//lowBorderPoint = -1.0;
		lowerVariation = OUT_OF_RANGE_NEGATIVE;
		upperVariation = MEDIUM_NEGATIVE;
	} else if (normalised_energy <= -0.5) {
		middleVariation = MEDIUM_NEGATIVE;
		midPoint = -0.65;
		//highBorderPoint = -0.5;
		//lowBorderPoint = -0.8;
		lowerVariation = HIGH_NEGATIVE;
		upperVariation = SMALL_NEGATIVE;
	} else if (normalised_energy <= -0.1) {
		middleVariation = SMALL_NEGATIVE;
		midPoint = -0.3;
		//highBorderPoint = -0.1;
		//lowBorderPoint = -0.5;
		lowerVariation = MEDIUM_NEGATIVE;
		upperVariation = STAGNANT_NEGATIVE;
	} else if (normalised_energy < 0 - DELTA) {
		middleVariation = STAGNANT_NEGATIVE;
		midPoint = -0.05;
		//highBorderPoint = 0;
		//lowBorderPoint = -0.1;
		lowerVariation = SMALL_NEGATIVE;
		upperVariation = ZERO;
	} else {
		// should never be here
		assert(false);
	}

	if ((middleVariation != OUT_OF_RANGE_POSITIVE) && (middleVariation != OUT_OF_RANGE_NEGATIVE)) {
		double raw_mid_point = normalised_energy - midPoint;
		double middle_category_fraction = std::max(1 - std::abs(raw_mid_point), 0.5);
		double upper_category_fraction = 0;
		double lower_category_fraction = 0;
		if (raw_mid_point >= 0) {
			upper_category_fraction = 1 - middle_category_fraction;
		} else {
			lower_category_fraction = 1 - middle_category_fraction;
		}

		// tidy up for out of bounds
		if ((upperVariation == OUT_OF_RANGE_POSITIVE) && (upper_category_fraction > 0)) {
			middle_category_fraction += upper_category_fraction;
			upper_category_fraction = 0;
		} else if ((lowerVariation == ZERO) && (lower_category_fraction > 0)) {
			middle_category_fraction += lower_category_fraction;
			lower_category_fraction = 0;
		} else if ((lowerVariation == OUT_OF_RANGE_NEGATIVE) && (lower_category_fraction > 0)) {
			middle_category_fraction += lower_category_fraction;
			lower_category_fraction = 0;
		} else if ((upperVariation == ZERO) && (upper_category_fraction > 0)) {
			middle_category_fraction += upper_category_fraction;
			upper_category_fraction = 0;
		}

		// assign
		evwp.variationMap[lowerVariation] = lower_category_fraction;
		evwp.variationMap[middleVariation] = middle_category_fraction;
		evwp.variationMap[upperVariation] = upper_category_fraction;

#ifdef CLUSTERANALYSERBASIC_DEBUG
		const std::map<EnergyVariation, double> & temp_map = evwp.variationMap;
		// forall in temp_map
		{
			double sum = 0;
			std::map<EnergyVariation, double>::const_iterator it_temp_map = temp_map.begin();
			const std::map<EnergyVariation, double>::const_iterator it_temp_map_end = temp_map.end();
			while (it_temp_map != it_temp_map_end) {
				sum += it_temp_map->second;
				++it_temp_map;
			}
			const double DELTA = 0.000001;
			bool check_sum = true;
			if (sum > 1 + DELTA) {
				check_sum = false;
				std::cerr << "ClusterAnalyserBasic::getEnergyVariationMap: ERROR: " << sum << ">1+DELTA" << std::endl;
			}
			if (sum < 1 - DELTA) {
				check_sum = false;
				std::cerr << "ClusterAnalyserBasic::getEnergyVariationMap: ERROR: " << sum << "<1-DELTA" << std::endl;
			}
			assert(check_sum);
		}
#endif
	}
	return evwp;
}

} /* namespace manipulators */
}/* namespace cryomesh */
