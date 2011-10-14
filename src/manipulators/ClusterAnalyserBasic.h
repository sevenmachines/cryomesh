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
	ClusterAnalyserBasic();
	virtual ~ClusterAnalyserBasic();
	virtual ClusterAnalysisData analyseCluster(const structures::Cluster & cluster, const std::map<int, std::list<ClusterAnalysisData> > & histories) const;

protected:
	ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const;
};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSERBASIC_H_ */
