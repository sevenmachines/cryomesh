/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#include "ClusterArchitect.h"
#include <algorithm>

namespace cryomesh {
namespace manipulators {

//STATICS
const int ClusterArchitect::DEFAULT_MAX_HISTORY_SIZE = 100;

ClusterArchitect::ClusterArchitect(structures::Cluster & clus) :
		cluster(clus), currentClusterAnalysisData(), minClusterAnalysisData(), maxClusterAnalysisData(), averageClusterAnalysisData(), maxHistorySize(
				ClusterArchitect::DEFAULT_MAX_HISTORY_SIZE) {
}

ClusterArchitect::~ClusterArchitect() {
}

void ClusterArchitect::runAnalysis() {
	// update history
	ClusterAnalysisData cad = this->analyseCluster();
	this->addHistoryEntry(cad);

	this->destroyRandomNodes(cad.getNodesToDestroy());
	this->birthRandomNodes(cad.getNodesToCreate());
	this->destroyRandomConnections(cad.getConnectionsToDestroy());
	this->birthRandomConnections(cad.getConnectionsToCreate());
}

const ClusterAnalysisData ClusterArchitect::analyseCluster() {
	// do checks on cluster state and set births/deaths accordingly
	//double current_energy = cluster.getEnergy();

	return ClusterAnalysisData(0, 0, 0, 0, 0);
}

void ClusterArchitect::birthRandomNodes(int count) {
	std::cout << "ClusterArchitect::birthRandomNodes: " << "count: " << count << std::endl;
	if (count > 0) {

	}
}

void ClusterArchitect::birthRandomConnections(int count) {
	std::cout << "ClusterArchitect::birthRandomConnections: " << "count: " << count << std::endl;
	if (count > 0) {

	}
}

void ClusterArchitect::destroyRandomNodes(int count) {
	std::cout << "ClusterArchitect::destroyRandomNodes: " << "count: " << count << std::endl;
	if (count > 0) {

	}
}

void ClusterArchitect::destroyRandomConnections(int count) {
	std::cout << "ClusterArchitect::destroyRandomConnections: " << "count: " << count << std::endl;
	if (count > 0) {

	}
}

std::vector<boost::shared_ptr<components::Node> > ClusterArchitect::getRandomNodes(int count) {
	std::vector<boost::shared_ptr<components::Node> > cont;
	if (count > 0) {
		cont = cluster.getMutableNodeMap().getRandomRange(count);
	}
	return cont;

}

const std::list<ClusterAnalysisData> & ClusterArchitect::getHistory() const {
	return history;
}

int ClusterArchitect::getMaxHistorySize() const {
	return maxHistorySize;
}

void ClusterArchitect::setMaxHistorySize(int sz) {
	if (sz > 0) {
		maxHistorySize = sz;
	} else {
		maxHistorySize = 1;
	}
}

void ClusterArchitect::addHistoryEntry(ClusterAnalysisData entry) {
	const unsigned int max_sz = static_cast<unsigned int>(this->getMaxHistorySize());

	history.push_back(entry);

	while (history.size() > max_sz) {
		history.pop_front();
	}
	currentClusterAnalysisData = entry;

	this->getHistoryStatistics(this->minClusterAnalysisData, this->maxClusterAnalysisData,
			this->averageClusterAnalysisData);
}
void ClusterArchitect::getHistoryStatistics(ClusterAnalysisData & minCad, ClusterAnalysisData & maxCad,
		ClusterAnalysisData & avCad) {

	ClusterAnalysisData sumCad;

	// forall in history
	{
		std::list<ClusterAnalysisData>::const_iterator it_history = history.begin();
		const std::list<ClusterAnalysisData>::const_iterator it_history_end = history.end();
		while (it_history != it_history_end) {
			sumCad += *it_history;
			if (minCad > *it_history) {
				minCad = *it_history;
			}
			if (maxCad < *it_history) {
				maxCad = *it_history;
			}
			++it_history;
		}
	}

	avCad = sumCad / history.size();
}

void ClusterArchitect::splitHistoryByValue(double db, int countback,
		std::map<common::Cycle, ClusterAnalysisData> & below,
		std::map<common::Cycle, ClusterAnalysisData> & above) const {
	if (countback <= 0) {
		countback = history.size();
	}

	// forall in history
	{
		std::list<ClusterAnalysisData>::const_reverse_iterator it_history = history.rbegin();
		const std::list<ClusterAnalysisData>::const_reverse_iterator it_history_end = history.rend();
		while ((it_history != it_history_end) && countback > 0) {
			if (it_history->getClusterEnergy() > db) {
				above[it_history->getCycle()] = *it_history;
			} else {
				below[it_history->getCycle()] = *it_history;
			}
			--countback;
			++it_history;
		}
	}
	assert(below.size() + above.size() == history.size());
}

std::vector<ClusterAnalysisData> ClusterArchitect::getHistoryEntriesInRange(double min_db, double max_db,
		int count) const {
	std::map<common::Cycle, ClusterAnalysisData> above_max;
	std::map<common::Cycle, ClusterAnalysisData> below_max;
	std::map<common::Cycle, ClusterAnalysisData> above_min;
	std::map<common::Cycle, ClusterAnalysisData> below_min;
	this->splitHistoryByValue(min_db, count, below_min, above_min);
	this->splitHistoryByValue(max_db, count, below_max, above_max);

	std::vector<ClusterAnalysisData> intersection;
	std::set_intersection(above_min.begin(), above_min.end(), below_max.begin(), below_max.end(), intersection.begin());
	return intersection;
}

} /* namespace manipulators */
} /* namespace cryomesh */
