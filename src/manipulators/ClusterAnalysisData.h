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
	ClusterAnalysisData();
	ClusterAnalysisData(double cluster_energy, int nodes_destroy, int cons_destroy, int nodes_create, int cons_create);
	ClusterAnalysisData(const ClusterAnalysisData & obj);
	virtual ~ClusterAnalysisData();

	/**
	 * Non-destructive addition operator
	 *
	 * @param const ClusterAnalysisData & obj
	 * 		RHS addition
	 *
	 * @return ClusterAnalysisData
	 * 		New object after addition
	 */
	const ClusterAnalysisData operator+(const ClusterAnalysisData & obj) const;
	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const ClusterAnalysisData & obj
	 * 		RHS addition
	 *
	 * @return ClusterAnalysisData &
	 * 		This object after addition and assignment
	 */
	ClusterAnalysisData & operator+=(const ClusterAnalysisData & obj);

	const ClusterAnalysisData operator/(const double dbl) const;
	ClusterAnalysisData & operator/=(const double dbl);

	/**
	 * Less than operator
	 *
	 * @param const ClusterAnalysisData & obj
	 * 		RHS
	 *
	 * @return bool
	 * 		True if < than obj, false otherwise
	 */
	bool operator<(const ClusterAnalysisData & obj) const;
	/**
	 * Greater than operator
	 *
	 * @param const ClusterAnalysisData & obj
	 * 		RHS
	 *
	 * @return bool
	 * 		True if > than obj, false otherwise
	 */
	bool operator>(const ClusterAnalysisData & obj) const;

	double getClusterEnergy() const;
	int getNodesToDestroy() const;
	int getConnectionsToDestroy() const;
	int getNodesToCreate() const;
	int getConnectionsToCreate() const;
	common::Cycle getCycle() const;

private:
	double clusterEnergy;
	int nodesToDestroy;
	int connectionsToDestroy;
	int nodesToCreate;
	int connectionsToCreate;
	common::Cycle cycle;

};

} /* namespace manipulators */
} /* namespace cryomesh */
#endif /* CLUSTERANALYSISDATA_H_ */
