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
	enum EnergyVariation{
		OUT_OF_RANGE_POSITIVE = 5,
		HIGH_POSITIVE = 4,
		MEDIUM_POSITIVE = 3,
		SMALL_POSITIVE = 2,
		STAGNANT_POSITIVE = 1,
		ZERO = 0,
		STAGNANT_NEGATIVE  = -1,
		SMALL_NEGATIVE = -2,
		MEDIUM_NEGATIVE = -3,
		HIGH_NEGATIVE  = -4,
		OUT_OF_RANGE_NEGATIVE = -5
	};
	ClusterAnalyserBasic();
	virtual ~ClusterAnalyserBasic();
	virtual ClusterAnalysisData analyseCluster(const structures::Cluster & cluster,
			const std::map<int, std::list<ClusterAnalysisData> > & histories) const;
	virtual ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const;

protected:
	virtual EnergyVariation getFuzzyEnergyVariation(double d, double range,  double fuzz_factor)const;
	virtual EnergyVariation getEnergyVariation(double d, double range)const;
};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSERBASIC_H_ */
