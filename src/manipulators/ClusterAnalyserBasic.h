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

class ClusterArchitect;

class ClusterAnalyserBasic: public cryomesh::manipulators::IClusterAnalyser {
public:

	ClusterAnalyserBasic(const ClusterArchitect & ca);
	virtual ~ClusterAnalyserBasic();
	virtual ClusterAnalysisData analyseCluster(const structures::Cluster & cluster,
			const std::map<int, std::list<ClusterAnalysisData> > & histories);
	virtual ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const;
	int getConnectionCreationEnabledCountdown() const;
	int getConnectionDestructionEnabledCountdown() const;
	int getNodeCreationEnabledCountdown() const;
	int getNodeDestructionEnabledCountdown() const;
	void setConnectionDestructionEnabledCountdown(int connectionDestructionEnabledCountdown);

protected:
	virtual EnergyVariationWeightingMap getEnergyVariationMap(const double energy_input, double range) const;

private:
	const ClusterArchitect & clusterArchitect;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSERBASIC_H_ */
