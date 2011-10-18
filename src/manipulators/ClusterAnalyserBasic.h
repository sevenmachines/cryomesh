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

	/**
	 * Class to hold together information on whether we can act to restructure items
	 */
	struct RestructuringCountdown {
		RestructuringCountdown(int ct=0) :
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
					&& (this->isAnyMediumRestructuringEnabled() || this->isAnyLongRestructuringEnabled()));
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
	RestructuringCountdown nodeRestructuring;
	RestructuringCountdown connectionRestructuring;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSERBASIC_H_ */
