/*
 * IClusterAnalyser.h
 *
 *  Created on: 12 Oct 2011
 *      Author: niall
 */

#ifndef ICLUSTERANALYSER_H_
#define ICLUSTERANALYSER_H_

#include "ClusterAnalysisData.h"
#include "structures/Cluster.h"

namespace cryomesh {
namespace manipulators {

class IClusterAnalyser {
public:

	/**
	 * Enum to represent the severity of the analysis
	 */
	enum AnalysisSeverity {
		NONE, VERYLOW, LOW, MEDIUM, HIGH, VERYHIGH, MAXIMUM
	};

	IClusterAnalyser() {
	}
	virtual ~IClusterAnalyser() {
	}
	/**
	 * Run an analysis on the cluster to decide what action to take on nodes and connections
	 *
	 * @param const structures::Cluster &
	 * 	The cluster to analyse
	 * @param	const std::list<ClusterAnalysisData>
	 * 	The historical analysis data to use
	 *
	 * @return ClusterAnalysisData
	 * 	The reulting analytical data
	 */
	virtual ClusterAnalysisData analyseCluster(const structures::Cluster & cluster,
			const std::map<int, std::list<ClusterAnalysisData> > & histories) =0;
	/**
	 * Calculate the range energies stats
	 *
	 * @param const std::list<ClusterAnalysisData> &
	 * 	The history range to work with
	 *
	 * @return ClusterAnalysisData
	 * 	The resulting cluster analysis data
	 */
	virtual ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history)=0;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* ICLUSTERANALYSER_H_ */
