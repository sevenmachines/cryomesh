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
	 * Class to hold together information on whether we can act to restructure items
	 */
	struct RestructuringCountdown {
		RestructuringCountdown(int ct = 0) :
				shortCreation(ct), shortDestruction(ct), mediumCreation(ct), mediumDestruction(ct), longCreation(ct), longDestruction(
						ct) {
		}
		bool isRestructuringEnabled(int var) const {
			if (var == 0) {
				return true;
			}
			return false;
		}

		bool isAnyShortRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->shortCreation)
					|| this->isRestructuringEnabled(this->shortDestruction));
		}
		bool isAnyMediumRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->mediumCreation)
					|| this->isRestructuringEnabled(this->mediumDestruction));
		}
		bool isAnyLongRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->longDestruction)
					|| this->isRestructuringEnabled(this->longCreation));
		}
		bool isAnyRestructuringEnabled() const {
			return ((this->isAnyShortRestructuringEnabled())
					|| (this->isAnyMediumRestructuringEnabled() || this->isAnyLongRestructuringEnabled()));
		}

		bool isAllShortRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->shortCreation)
					&& this->isRestructuringEnabled(this->shortDestruction));
		}
		bool isAllMediumRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->mediumCreation)
					&& this->isRestructuringEnabled(this->mediumDestruction));
		}
		bool isAllLongRestructuringEnabled() const {
			return (this->isRestructuringEnabled(this->longDestruction)
					&& this->isRestructuringEnabled(this->longCreation));
		}
		bool isAllRestructuringEnabled() const {
			return ((this->isAnyShortRestructuringEnabled())
					&& (this->isAnyMediumRestructuringEnabled() && this->isAnyLongRestructuringEnabled()));
		}

		/**
		 * Prefix decrement operator
		 *
		 * @return RestructuringCountdown &
		 * 		Return this
		 */
		RestructuringCountdown & operator--() {
			if (shortCreation > 0) {
				--shortCreation;
			}
			if (shortDestruction > 0) {
				--shortDestruction;
			}
			if (mediumCreation > 0) {
				--mediumCreation;
			}
			if (mediumDestruction > 0) {
				--mediumDestruction;
			}
			if (longCreation > 0) {
				--longCreation;
			}
			if (longDestruction > 0) {
				--longDestruction;
			}
			return *this;
		}

		void setShortCountdown(int ct) {
			shortCreation = ct;
			shortDestruction = ct;
		}
		void setMediumCountdown(int ct) {
			mediumCreation = ct;
			mediumDestruction = ct;
		}
		void setLongCountdown(int ct) {
			longCreation = ct;
			longDestruction = ct;
		}

		/**
		 * To stream operator
		 *
		 *	@param std::ostream & os
		 *		The output stream
		 *	@param const RestructuringCountdown & obj
		 *		The object to stream
		 *
		 *	@return std::ostream &
		 *		The output stream
		 */
		friend std::ostream& operator<<(std::ostream & os, const RestructuringCountdown & obj) {
			os << "RestructuringCountdown: ";
			os << "{ ( " << obj.shortCreation << ", " << obj.shortDestruction << " )" ;
			os << ", ( " << obj.mediumCreation << ", " << obj.mediumDestruction << " )" ;
			os << ", ( " << obj.longCreation << ", " << obj.longDestruction << " )}" ;
			return os;
		}
		int shortCreation;
		int shortDestruction;
		int mediumCreation;
		int mediumDestruction;
		int longCreation;
		int longDestruction;

	};
	enum EnergyVariation {
		NONE = 2048,
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
		EnergyVariationWeightingMap() :
				variationMap() {
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

	IClusterAnalyser() :
			nodeRestructuring(), connectionRestructuring() {
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
	virtual ClusterAnalysisData calculateRangeEnergies(const std::list<ClusterAnalysisData> & history) const =0;
	virtual EnergyVariationWeightingMap getEnergyVariationMap(const double energy_input, double range) const=0;

	const RestructuringCountdown & getConnectionRestructuring() const {
		return connectionRestructuring;
	}
	const RestructuringCountdown &getNodeRestructuring() const {
		return nodeRestructuring;
	}
protected:
	RestructuringCountdown nodeRestructuring;
	RestructuringCountdown connectionRestructuring;
};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* ICLUSTERANALYSER_H_ */
