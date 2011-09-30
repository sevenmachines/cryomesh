/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#include "ClusterArchitect.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <components/Node.h>
#include <components/NodeMap.h>

#include <algorithm>

namespace cryomesh {
namespace manipulators {

//STATICS
const int ClusterArchitect::DEFAULT_MAX_HISTORY_SIZE = 100;
const double ClusterArchitect::DEFAULT_CONNECTIVITY_FRACTION = 0.01;

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

void ClusterArchitect::birthRandomNodes(int count, int connectivity) {
	std::cout << "ClusterArchitect::birthRandomNodes: " << "count: " << count << std::endl;

	if (count > 0) {
		components::NodeMap & nmap = cluster.getMutableNodeMap();
		std::vector<boost::shared_ptr<cryomesh::components::Node> > new_nodes;
		for (int i = 0; i < count; i++) {
			boost::shared_ptr<components::Node> tempnode = components::Node::getRandom(cluster.getMaxBoundingBox());
			new_nodes.push_back(tempnode);
			nmap.add(tempnode);
		}

		//  now connect up new nodes
		std::vector<boost::shared_ptr<cryomesh::components::Node> > node_range =
				cluster.getMutableNodeMap().getRandomRange(new_nodes.size());

		 unsigned int default_connectivity = connectivity;

		// if connectivity is 0 then use default fraction
		if (connectivity <= 0) {
			double node_count_fraction = cluster.getNodeMap().getSize()
					* ClusterArchitect::DEFAULT_CONNECTIVITY_FRACTION;
			int node_count_fraction_ceil = static_cast<int>(std::ceil(node_count_fraction));
			default_connectivity = std::max(1, node_count_fraction_ceil);
		}
		// forall in new_nodes
		{
			std::vector<boost::shared_ptr<cryomesh::components::Node> >::iterator it_new_nodes = new_nodes.begin();
			const std::vector<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
					new_nodes.end();
			std::vector<boost::shared_ptr<cryomesh::components::Node> >::iterator it_node_range = node_range.begin();
			const std::vector<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_node_range_end =
					node_range.end();
			while (it_new_nodes != it_new_nodes_end) {
				unsigned int new_in_count = 0;
				unsigned int new_out_count = 0;

				// make multiple input connections
				while (new_in_count < default_connectivity) {
					if (it_node_range == it_node_range_end) {
						it_node_range = node_range.begin();
					}
					// connection multiplicity of 1
					this->createConnection(*it_node_range, *it_new_nodes, 1);
					++it_node_range;
				}

				// make multiple output connections
				while (new_out_count < default_connectivity) {
					if (it_node_range == it_node_range_end) {
						it_node_range = node_range.begin();
					}
					// connection multiplicity of 1
					this->createConnection(*it_new_nodes, *it_node_range, 1);
					++it_node_range;
				}

				assert((*it_new_nodes)->getConnector().getInputs().size() == default_connectivity);
				assert((*it_new_nodes)->getConnector().getOutputs().size() == default_connectivity);
				++it_new_nodes;
			}
		}
	}
}

void ClusterArchitect::birthRandomConnections(int count) {
	std::cout << "ClusterArchitect::birthRandomConnections: " << "count: " << count << std::endl;
	if (count > 0) {
		components::NodeMap & nmap = cluster.getMutableNodeMap();
		components::ConnectionMap & cmap = cluster.getMutableConnectionMap();

		std::vector<boost::shared_ptr<components::Node> > source_nodes = nmap.getRandomRange(count);
		std::vector<boost::shared_ptr<components::Node> > dest_nodes = nmap.getRandomRange(count);

		assert(source_nodes.size() == dest_nodes.size());

		// forall in source_nodes
		{
			std::vector<boost::shared_ptr<components::Node> >::iterator it_source_nodes = source_nodes.begin();
			const std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end =
					source_nodes.end();
			std::vector<boost::shared_ptr<components::Node> >::iterator it_dest_nodes = dest_nodes.begin();
			while (it_source_nodes != it_source_nodes_end) {
				boost::shared_ptr<components::Connection> tempcon(new components::Connection);

				tempcon->getMutableConnector().connectInput(*it_source_nodes);
				tempcon->getMutableConnector().connectOutput(*it_dest_nodes);

				(*it_source_nodes)->getMutableConnector().connectOutput(tempcon);
				(*it_dest_nodes)->getMutableConnector().connectInput(tempcon);

				cmap.add(tempcon);
				++it_source_nodes;
				++it_dest_nodes;
			}
		}

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

	// forall in above_min
	{
		std::map<common::Cycle, ClusterAnalysisData>::const_iterator it_above_min = above_min.begin();
		const std::map<common::Cycle, ClusterAnalysisData>::const_iterator it_above_min_end = above_min.end();
		while (it_above_min != it_above_min_end) {
			std::map<common::Cycle, ClusterAnalysisData>::iterator it_found = below_max.find(it_above_min->first);
			if (it_found != below_max.end()) {
				assert(it_found->second.getUUID() == it_above_min->second.getUUID());
				intersection.push_back(it_above_min->second);
			}
			++it_above_min;
		}
	}
	return intersection;
}

void ClusterArchitect::createConnection(boost::shared_ptr<components::Node> nodeStart,
		boost::shared_ptr<components::Node> nodeEnd, int connectivity) {

	for (int i = 0; i < connectivity; i++) {

		boost::shared_ptr<components::Connection> tempcon(new components::Connection);

		tempcon->getMutableConnector().connectInput(nodeStart);
		tempcon->getMutableConnector().connectOutput(nodeEnd);

		nodeStart->getMutableConnector().connectOutput(tempcon);
		nodeEnd->getMutableConnector().connectInput(tempcon);

		cluster.getMutableConnectionMap().add(tempcon);

	}
}

} /* namespace manipulators */
} /* namespace cryomesh */
