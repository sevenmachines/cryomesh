/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#include "ClusterArchitect.h"

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
}

const ClusterAnalysisData ClusterArchitect::analyseCluster() {
	return ClusterAnalysisData(0, 0, 0, 0);
}

void ClusterArchitect::birthRandomNodes(int count) {
	std::cout << "ClusterArchitect::birthRandomNodes: " << "count: " << count << std::endl;
}

void ClusterArchitect::birthRandomConnections(int count) {
	std::cout << "ClusterArchitect::birthRandomConnections: " << "count: " << count << std::endl;
}

void ClusterArchitect::destroyRandomNodes(int count) {
	std::cout << "ClusterArchitect::destroyRandomNodes: " << "count: " << count << std::endl;
}

void ClusterArchitect::destroyRandomConnections(int count) {
	std::cout << "ClusterArchitect::destroyRandomConnections: " << "count: " << count << std::endl;
}

std::list<ClusterAnalysisData>  ClusterArchitect::getRandomNodes(int count){
	std::cout << "ClusterArchitect::getRandomNodes: " << "count: " << count << std::endl;
	std::list<ClusterAnalysisData> cont;
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
	const unsigned int max_sz =  static_cast<unsigned int>(this->getMaxHistorySize());

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
			if ( minCad > *it_history){
				minCad = *it_history;
			}
			if (maxCad < *it_history){
				maxCad = *it_history;
			}
			++it_history;
		}
	}

	avCad = sumCad / history.size();
}

} /* namespace manipulators */
} /* namespace cryomesh */
