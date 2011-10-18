/*
 * ClusterAnalysisData.h
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#ifndef CLUSTERANALYSISDATA_H_
#define CLUSTERANALYSISDATA_H_

#include "common/Tagged.h"
#include "common/Cycle.h"
#include "common/TimeKeeper.h"

namespace cryomesh {
namespace manipulators {

class ClusterAnalysisData: public common::Tagged {
public:

	/**
	 * Struct representing the value extrapolated over a history range
	 */
	struct RangeEnergy {
		RangeEnergy() :
				energy(0), startCycle(0), endCycle(0), energyMin(0), energyMax(0) {
		}
		RangeEnergy(double en) :
				energy(en), startCycle(common::TimeKeeper::getTimeKeeper().getCycle()), endCycle(
						common::TimeKeeper::getTimeKeeper().getCycle()), energyMin(en), energyMax(en) {
		}
		RangeEnergy(double en, common::Cycle st, common::Cycle ed, double min, double max) :
				energy(en), startCycle(st), endCycle(ed), energyMin(min), energyMax(max) {
		}

		RangeEnergy(const RangeEnergy & obj) :
				energy(obj.energy), startCycle(obj.startCycle), endCycle(obj.endCycle), energyMin(obj.energyMin), energyMax(
						obj.energyMax) {
		}
		virtual ~RangeEnergy() {
		}
		/**
		 * Non-destructive addition operator
		 *
		 * @param const RangeEnergy & obj
		 * 		RHS addition
		 *
		 * @return RangeEnergy
		 * 		New object after addition
		 */
		const RangeEnergy operator+(const RangeEnergy & obj) const {
			RangeEnergy temp_this(*this);
			temp_this += obj;
			return temp_this;
		}
		/**
		 * Destructive addition and assignment operator
		 *
		 * @param const RangeEnergy & obj
		 * 		RHS addition
		 *
		 * @return RangeEnergy &
		 * 		This object after addition and assignment
		 */
		RangeEnergy & operator+=(const RangeEnergy & obj) {
			this->energy += obj.energy;
			this->energyMin = std::min(this->energyMin, obj.energyMin);
			this->energyMax = std::max(this->energyMax, obj.energyMax);

			this->startCycle = std::min(this->startCycle, obj.startCycle);
			this->endCycle = std::max(this->endCycle, obj.endCycle);
			return *this;
		}

		const RangeEnergy operator/(double div) const {
			RangeEnergy temp_this(*this);
			temp_this /= div;
			return temp_this;
		}
		RangeEnergy & operator/=(double div) {
			this->energy /= div;
			return *this;
		}

		/**
		 * To stream operator
		 *
		 *	@param std::ostream & os
		 *		The output stream
		 *	@param const RangeEnergy & obj
		 *		The object to stream
		 *
		 *	@return std::ostream &
		 *		The output stream
		 */
		friend std::ostream& operator<<(std::ostream & os, const RangeEnergy & obj) {
			os << "RangeEnergy: { " << "energy=" << obj.energy << " startCycle=" <<  obj.startCycle << " endCycle=" <<  obj.endCycle
					<< " energyMin=" <<  obj.energyMin << " energyMax=" <<  obj.energyMax<<" }";
			return os;
		}
		double energy;
		common::Cycle startCycle;
		common::Cycle endCycle;
		double energyMin;
		double energyMax;
	};

	ClusterAnalysisData();
	ClusterAnalysisData(RangeEnergy cre);
	ClusterAnalysisData(RangeEnergy clusterRangeEnergy, double node_creation_weight, double node_destruction_weight,
			double conn_creation_weight, double conn_destruction_weight, int node_create, int nodes_destroy,
			int conn_create, int conn_destroy);
	ClusterAnalysisData(const ClusterAnalysisData & obj);
	virtual ~ClusterAnalysisData();

	RangeEnergy getClusterRangeEnergy() const;
	double getNodeCreationWeight() const;
	double getNodeDestructionWeight() const;
	double getConnectionCreationWeight() const;
	double getConnectionDestructionWeight() const;
	int getNodesToDestroy() const;
	int getConnectionsToDestroy() const;
	int getNodesToCreate() const;
	int getConnectionsToCreate() const;
	void setClusterRangeEnergy(RangeEnergy clusterEnergy);
	void setConnectionCreationWeight(double connectionCreationWeight);
	void setConnectionDestructionWeight(double connectionDestructionWeight);
	void setConnectionsToCreate(int connectionsToCreate);
	void setConnectionsToDestroy(int connectionsToDestroy);
	void setNodeCreationWeight(double nodeCreationWeight);
	void setNodeDestructionWeight(double nodeDestructionWeight);
	void setNodesToCreate(int nodesToCreate);
	void setNodesToDestroy(int nodesToDestroy);

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const ClusterAnalysisData & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const ClusterAnalysisData & obj) {
		os << "ClusterAnalysisData: " << "{ " << "clusterRangeEnergy=" <<  obj.clusterRangeEnergy << " nodeCreationWeight="
				<<  obj.nodeCreationWeight << " nodeDestructionWeight=" <<  obj.nodeDestructionWeight
				<< " connectionCreationWeight=" <<  obj.connectionCreationWeight << " connectionDestructionWeight="
				<<  obj.connectionDestructionWeight << " nodesToCreate=" <<  obj.nodesToCreate << " nodesToDestroy="
				<< obj. nodesToDestroy << " connectionsToCreate=" <<  obj.connectionsToCreate << " connectionsToDestroy="
				<<  obj.connectionsToDestroy << " }";
		return os;
	}
private:
	RangeEnergy clusterRangeEnergy;
	/**
	 * Weight given to creating nodes [0,1]
	 *
	 * @var double
	 */
	double nodeCreationWeight;
	/**
	 * Weight given to destroying nodes [0,1]
	 *
	 * @var double
	 */
	double nodeDestructionWeight;
	/**
	 * Weight given to creating connections [0,1]
	 *
	 * @var double
	 */
	double connectionCreationWeight;
	/**
	 * Weight given to destroying connections [0,1]
	 *
	 * @var double
	 */
	double connectionDestructionWeight;

	int nodesToCreate;
	int nodesToDestroy;
	int connectionsToCreate;
	int connectionsToDestroy;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSISDATA_H_ */
