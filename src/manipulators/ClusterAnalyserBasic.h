/*
 * ClusterAnalyserBasic.h
 *
 *  Created on: 12 Oct 2011
 *      Author: niall
 */

#ifndef CLUSTERANALYSERBASIC_H_
#define CLUSTERANALYSERBASIC_H_

#include "IClusterAnalyser.h"

namespace cryomesh {
namespace manipulators {

class ClusterAnalyserBasic: public cryomesh::manipulators::IClusterAnalyser {
public:
	enum EnergyVariation {
		NONE= 2048,
		OUT_OF_RANGE_POSITIVE = 1024,
		HIGH_POSITIVE = 512,
		MEDIUM_POSITIVE = 256,
		SMALL_POSITIVE = 128,
		STAGNANT_POSITIVE = 64,
		ZERO = 32,
		STAGNANT_NEGATIVE = 16,
		SMALL_NEGATIVE = 8,
		MEDIUM_NEGATIVE = 4,
		HIGH_NEGATIVE = 2,
		OUT_OF_RANGE_NEGATIVE = 1
	};
	struct EnergyVariationWeightingMap {
		EnergyVariationWeightingMap():variationMap(){
			variationMap[OUT_OF_RANGE_POSITIVE] = 0.0;
			variationMap[HIGH_POSITIVE] = 0.0;
			variationMap[MEDIUM_POSITIVE] = 0.0;
			variationMap[SMALL_POSITIVE] = 0.0;
			variationMap[STAGNANT_POSITIVE] = 0.0;
			variationMap[ZERO] = 0.0;
			variationMap[STAGNANT_NEGATIVE] = 0.0;
			variationMap[SMALL_NEGATIVE] = 0.0;
			variationMap[MEDIUM_NEGATIVE] = 0.0;
			variationMap[HIGH_NEGATIVE] = 0.0;
			variationMap[OUT_OF_RANGE_NEGATIVE] = 0.0;
		}
		std::map<EnergyVariation, double> variationMap;
	};

	ClusterAnalyserBasic();
	virtual ~ClusterAnalyserBasic();
	virtual ClusterAnalysisData analyseCluster(const structures::Cluster & cluster,
			const std::map<int, std::list<ClusterAnalysisData> > & histories) const;
	virtual ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const;

protected:
	virtual EnergyVariationWeightingMap getEnergyVariationMap(const double energy_input, double range) const;
};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSERBASIC_H_ */
