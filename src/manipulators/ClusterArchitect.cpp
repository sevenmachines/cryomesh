/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#include "ClusterArchitect.h"

namespace cryomesh {
namespace manipulators {

ClusterArchitect::ClusterArchitect(structures::Cluster & clus) :
		cluster(clus) {

}

ClusterArchitect::~ClusterArchitect() {
}

void ClusterArchitect::runAnalysis() {
}

const ClusterArchitect::ClusterAnalysisData ClusterArchitect::analyseCluster() {
	return ClusterAnalysisData(0,0,0,0);
}

void ClusterArchitect::birthRandomNodes(int count) {
std::cout<<"ClusterArchitect::birthRandomNodes: "<<"count: "<<count<<std::endl;
}

void ClusterArchitect::birthRandomConnections(int count) {
	std::cout<<"ClusterArchitect::birthRandomConnections: "<<"count: "<<count<<std::endl;
}

void ClusterArchitect::destroyRandomNodes(int count) {
	std::cout<<"ClusterArchitect::destroyRandomNodes: "<<"count: "<<count<<std::endl;
}

void ClusterArchitect::destroyRandomConnections(int count) {
	std::cout<<"ClusterArchitect::destroyRandomConnections: "<<"count: "<<count<<std::endl;
}

std::vector<components::Node> ClusterArchitect::getRandomNodes(int count) {
	std::cout<<"ClusterArchitect::getRandomNodes: "<<"count: "<<count<<std::endl;
	std::vector<components::Node> vec;
	return vec;
}

} /* namespace manipulators */
} /* namespace cryomesh */
