/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

#define CLUSTERARCHITECT_DEBUG

#include "ClusterArchitect.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <components/Node.h>
#include <components/NodeMap.h>
#include "structures/Fibre.h"

#include <algorithm>

namespace cryomesh {
namespace manipulators {

//STATICS
const int ClusterArchitect::DEFAULT_MAX_HISTORY_SIZE = 100;
const double ClusterArchitect::DEFAULT_CONNECTIVITY_FRACTION = 0.01;

ClusterArchitect::ClusterArchitect(structures::Cluster & clus) :
		cluster(clus), history(), currentClusterAnalysisData(), minClusterAnalysisData(), maxClusterAnalysisData(), averageClusterAnalysisData(), maxHistorySize(
				ClusterArchitect::DEFAULT_MAX_HISTORY_SIZE) {
}

ClusterArchitect::~ClusterArchitect() {
}

void ClusterArchitect::runAnalysis() {
	// update history
	ClusterAnalysisData cad = this->analyseCluster();
	this->addHistoryEntry(cad);

	this->destroyRandomNodes(cad.getNodesToDestroy());
	this->createRandomNodes(cad.getNodesToCreate());
	this->destroyRandomConnections(cad.getConnectionsToDestroy());
	this->createRandomConnections(cad.getConnectionsToCreate());
}

const ClusterAnalysisData ClusterArchitect::analyseCluster() {
	// do checks on cluster state and set births/deaths accordingly
	//double current_energy = cluster.getEnergy();

	return ClusterAnalysisData(0, 0, 0, 0, 0);
}

std::list<boost::shared_ptr<cryomesh::components::Node> > ClusterArchitect::createRandomNodes(int count,
		int connectivity, int strategy) {
#ifdef CLUSTERARCHITECT_DEBUG
	std::cout << "ClusterArchitect::createRandomNodes: " << "( " << count << ", " << connectivity << ", ";
	if (strategy & ClusterArchitect::ENABLE_SELF_CONNECT) {
		std::cout << "ENABLE_SELF_CONNECT ";
	}
	if (strategy & ClusterArchitect::ENABLE_EVEN_DISTRIBUTION) {
		std::cout << "ENABLE_EVEN_DISTRIBUTION ";
	}
	std::cout << ")" << std::endl;
#endif
	unsigned int pre_nodes_sz = cluster.getNodeMap().getSize();

	// new nodes that will be created
	std::list<boost::shared_ptr<cryomesh::components::Node> > new_nodes;

	if (count > 0) {

		// Acquire resources from cluster and create the new nodes
		components::NodeMap & nmap = cluster.getMutableNodeMap();

		// if connectivity is <= 0 then use default fraction
		if (connectivity <= 0) {
			double node_count_fraction = nmap.getSize() * ClusterArchitect::DEFAULT_CONNECTIVITY_FRACTION;
			int node_count_fraction_ceil = static_cast<int>(std::ceil(node_count_fraction));
			connectivity = std::max(1, node_count_fraction_ceil);
		}

		for (int i = 0; i < count; i++) {
			boost::shared_ptr<components::Node> tempnode = components::Node::getRandom(cluster.getMaxBoundingBox());
			new_nodes.push_back(tempnode);
		}

		// if there are enough nodes in the cluster bulk already then delay adding the new ones
		// so we connect up only with the old. Else we add the new nodes to the bulk first then
		// connect amongst themselves

		int nmap_sz = nmap.getSize();
		bool new_in_bulk = (nmap_sz < (count * connectivity));

		if (new_in_bulk == true) {
			// now add the new nodes to the bulk
			nmap.add(new_nodes);
		}
		assert(nmap.getSize() > 0);

		// if we've only got one node, we need to self connect
		if (nmap.getSize() == 1) {
			strategy = strategy | ConnectionStrategy::ENABLE_SELF_CONNECT;
			std::cout << "ClusterArchitect::createRandomNodes: "
					<< "WARNING: Forced self connection on single node cluster..." << std::endl;
		}

		// Connect up new nodes
		{

			//connect up inputs
			{
				// Get a range from the cluster bulk, attempt to get enough individuals for unique
				// connections for both input and output for all new nodes
				std::vector<boost::shared_ptr<cryomesh::components::Node> > node_range =
						cluster.getMutableNodeMap().getRandomRange(connectivity * new_nodes.size());

				// forall in new_nodes
				{
					std::list<boost::shared_ptr<cryomesh::components::Node> >::iterator it_new_nodes =
							new_nodes.begin();
					const std::list<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
							new_nodes.end();
					std::vector<boost::shared_ptr<cryomesh::components::Node> >::iterator it_node_range =
							node_range.begin();
					const std::vector<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_node_range_end =
							node_range.end();
					while (it_new_nodes != it_new_nodes_end) {
						// Get current input count, needed so that if a new node has been connected to other new nodes
						// then that new connection is counted towards its connectivity
						int current_in_count = (*it_new_nodes)->getConnector().getInputs().size();

						// make multiple input connections
						int range_cycled_count = 0;
						while (current_in_count < connectivity) {
							if (it_node_range == it_node_range_end) {
								it_node_range = node_range.begin();
								++range_cycled_count;
							}
							// connection multiplicity of 1
							boost::shared_ptr<components::Node> temp_source_node = *it_node_range;
							boost::shared_ptr<components::Node> temp_dest_node = *it_new_nodes;
							assert(temp_source_node != 0);
							assert(temp_dest_node != 0);

							// if the new nodes arent in bulk then connect away

							// if we're allowed to self connect or source and dest are not the same
							bool self_connect = (strategy & ConnectionStrategy::ENABLE_SELF_CONNECT);
							bool not_same = (temp_source_node->getUUID() != temp_dest_node->getUUID());

							// if we dont need to connect evenly or the source has below connectivity
							bool evenly_distribute = (strategy & ENABLE_EVEN_DISTRIBUTION);
							bool node_has_capacity = (temp_source_node->getConnector().getOutputs().size()
									< static_cast<unsigned int>(connectivity));
#ifdef CLUSTERARCHITECT_DEBUG
							if (strategy == 0) {
								assert(!self_connect);
								assert(!evenly_distribute);
							}
#endif
							if ((!self_connect || not_same) && (!evenly_distribute || node_has_capacity)) {
								this->createConnection(temp_source_node, temp_dest_node, 1);
								++current_in_count;
							}

							assert(range_cycled_count < (connectivity * count));
							++it_node_range;

						}
						++it_new_nodes;
					}
				} // all in new nodes
			} // connect up inputs

			// connect outputs
			{
				// Get a range from the cluster bulk, attempt to get enough individuals for unique
				// connections for both input and output for all new nodes
				std::vector<boost::shared_ptr<cryomesh::components::Node> > node_range =
						cluster.getMutableNodeMap().getRandomRange(connectivity * new_nodes.size());

				// forall in new_nodes
				{
					std::list<boost::shared_ptr<cryomesh::components::Node> >::iterator it_new_nodes =
							new_nodes.begin();
					const std::list<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
							new_nodes.end();
					std::vector<boost::shared_ptr<cryomesh::components::Node> >::iterator it_node_range =
							node_range.begin();
					const std::vector<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_node_range_end =
							node_range.end();
					while (it_new_nodes != it_new_nodes_end) {
						// Get current output count, needed so that if a new node has been connected to other new nodes
						// then that new connection is counted towards its connectivity
						int current_out_count = (*it_new_nodes)->getConnector().getOutputs().size();

						//reset range cycled count
						int range_cycled_count = 0;
						// make multiple output connections
						while (current_out_count < connectivity) {
							if (it_node_range == it_node_range_end) {
								it_node_range = node_range.begin();
								++range_cycled_count;
							}
							// connection multiplicity of 1
							boost::shared_ptr<components::Node> temp_source_node = *it_new_nodes;
							boost::shared_ptr<components::Node> temp_dest_node = *it_node_range;
							assert(temp_source_node != 0);
							assert(temp_dest_node != 0);

							// if we're allowed to self connect or source and dest are not the same
							bool self_connect = (strategy & ConnectionStrategy::ENABLE_SELF_CONNECT);
							bool not_same = (temp_source_node->getUUID() != temp_dest_node->getUUID());

							// if we dont need to connect evenly or the source has below connectivity
							bool evenly_distribute = (strategy & ENABLE_EVEN_DISTRIBUTION);
							bool node_has_capacity = (temp_dest_node->getConnector().getInputs().size()
									< static_cast<unsigned int>(connectivity));
#ifdef CLUSTERARCHITECT_DEBUG
							if (strategy == 0) {
								assert(!self_connect);
								assert(!evenly_distribute);
							}
#endif
							if ((!self_connect || not_same) && (!evenly_distribute || node_has_capacity)) {
								this->createConnection(temp_source_node, temp_dest_node, 1);
								++current_out_count;
							}
							assert(range_cycled_count < (connectivity * count));
							++it_node_range;
						}
#ifdef CLUSTERARCHITECT_DEBUG
						int post_inputs_sz = (*it_new_nodes)->getConnector().getInputs().size();
						int post_outputs_sz = (*it_new_nodes)->getConnector().getOutputs().size();
						if (new_in_bulk == false) {
							assert(post_inputs_sz == connectivity);
							assert(post_outputs_sz == connectivity);
						} else {
							assert(post_inputs_sz >= connectivity);
							assert(post_outputs_sz >= connectivity);
						}
#endif
						++it_new_nodes;

					}
				} // FORALL NEW_NODES
			} //CONNECT OUTPUTS
		} // CONNECT UP NODES

#ifdef CLUSTERARCHITECT_DEBUG
		{
			if (new_in_bulk == true && pre_nodes_sz == 0) {
				// forall in new_nodes
				{
					std::list<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes =
							new_nodes.begin();
					const std::list<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
							new_nodes.end();
					while (it_new_nodes != it_new_nodes_end) {
						int post_inputs_sz = (*it_new_nodes)->getConnector().getInputs().size();
						int post_outputs_sz = (*it_new_nodes)->getConnector().getOutputs().size();
						std::cout << "ClusterArchitect::createRandomNodes: " << (*it_new_nodes)->getUUIDSummary()
								<< " (" << post_inputs_sz << ", " << post_outputs_sz << ")" << std::endl;
						//assert(
						//		(*it_new_nodes)->getConnector().getInputs().size()
						//				== static_cast<unsigned int>(connectivity));
						//assert(post_outputs_sz == connectivity);
						++it_new_nodes;
					}
				}
			}

		}
#endif

// If we didnt add the new nodes to the bulk before connection then add them now
		if (new_in_bulk == false) {
			// now add the new nodes to the bulk
			nmap.add(new_nodes);
		}
#ifdef CLUSTERARCHITECT_DEBUG
		unsigned int post_nodes_sz = cluster.getNodeMap().getSize();
		assert(new_nodes.size() == static_cast<unsigned int>(count));
		assert(post_nodes_sz = pre_nodes_sz + count);
#endif
	}
	return new_nodes;
}

std::list<boost::shared_ptr<components::Connection> > ClusterArchitect::createRandomConnections(int count) {

	std::list<boost::shared_ptr<components::Connection> > new_connections;

	if (count > 0) {
		components::NodeMap & nmap = cluster.getMutableNodeMap();
		components::ConnectionMap & cmap = cluster.getMutableConnectionMap();

		std::vector<boost::shared_ptr<components::Node> > source_nodes = nmap.getRandomRange(count);
		std::vector<boost::shared_ptr<components::Node> > dest_nodes = nmap.getRandomRange(count);

		assert(source_nodes.size() == dest_nodes.size());

		// forall in source_nodes
		if ((source_nodes.size() > 0) && dest_nodes.size() > 0) {
			std::vector<boost::shared_ptr<components::Node> >::iterator it_source_nodes = source_nodes.begin();
			std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end = source_nodes.end();
			std::vector<boost::shared_ptr<components::Node> >::iterator it_dest_nodes = dest_nodes.begin();
			std::vector<boost::shared_ptr<components::Node> >::const_iterator it_dest_nodes_end = dest_nodes.end();

			unsigned int ui_count = static_cast<unsigned int>(count);
			while (new_connections.size() < ui_count) {
				if (it_source_nodes == it_source_nodes_end) {
					source_nodes = nmap.getRandomRange(count);
					it_source_nodes = source_nodes.begin();

				}
				if (it_dest_nodes == it_dest_nodes_end) {
					dest_nodes = nmap.getRandomRange(count);
					it_dest_nodes = dest_nodes.begin();
					it_dest_nodes_end = dest_nodes.end();
				}

				boost::shared_ptr<components::Connection> tempcon(new components::Connection);

				tempcon->getMutableConnector().connectInput(*it_source_nodes);
				tempcon->getMutableConnector().connectOutput(*it_dest_nodes);

				(*it_source_nodes)->getMutableConnector().connectOutput(tempcon);
				(*it_dest_nodes)->getMutableConnector().connectInput(tempcon);

				cmap.add(tempcon);
				new_connections.push_back(tempcon);
				++it_source_nodes;
				++it_dest_nodes;
			}
		}

	}

#ifdef CLUSTERARCHITECT_DEBUG
	const int new_connections_size = new_connections.size();
	assert(new_connections_size == static_cast<unsigned int>(count));
#endif
	return new_connections;
}

std::list<boost::shared_ptr<components::Node> > ClusterArchitect::destroyRandomNodes(int count) {
	std::cout << "ClusterArchitect::destroyRandomNodes: " << "count: " << count << std::endl;
	std::list<boost::shared_ptr<components::Node> > dead_nodes;

	if (count > 0) {

		// get a random selection of nodes
		std::list<boost::shared_ptr<components::Node> > source_nodes = this->getRandomNodes(count, false);

		// forall in source_nodes
		{
			std::list<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes = source_nodes.begin();
			const std::list<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end =
					source_nodes.end();
			while (it_source_nodes != it_source_nodes_end) {
				// get inputs and delete them from cluster
				cluster.getMutableConnectionMap().remove((*it_source_nodes)->getConnector().getInputsUUID());
				// now disconnect node
				(*it_source_nodes)->destroyAllConnections();
				dead_nodes.push_back(*it_source_nodes);
				++it_source_nodes;
			}
		}
#ifdef CLUSTERARCHITECT_DEBUG
		const unsigned int PRE_CLUSTER_NODES_SZ = cluster.getNodeMap().getSize();
#endif
		// remove dead nodes from cluster
		cluster.getMutableNodeMap().remove(dead_nodes);
#ifdef CLUSTERARCHITECT_DEBUG
		const unsigned int POST_CLUSTER_NODES_SZ = cluster.getNodeMap().getSize();
		assert(PRE_CLUSTER_NODES_SZ - dead_nodes.size() == POST_CLUSTER_NODES_SZ);
#endif
	}

	return dead_nodes;
}

std::list<boost::shared_ptr<components::Connection> > ClusterArchitect::destroyRandomConnections(int count) {
	std::cout << "ClusterArchitect::destroyRandomConnections: " << "count: " << count << std::endl;
	std::list<boost::shared_ptr<components::Connection> > dead_connections;
	if (count > 0) {
		std::list<boost::shared_ptr<components::Connection> > rand_conns = this->getRandomConnections(count, false);
		// forall in rand_conns
		{
			std::list<boost::shared_ptr<components::Connection> >::iterator it_rand_conns = rand_conns.begin();
			const std::list<boost::shared_ptr<components::Connection> >::const_iterator it_rand_conns_end =
					rand_conns.end();
			while (it_rand_conns != it_rand_conns_end) {
				// delete
				(*it_rand_conns)->disconnect();
				++it_rand_conns;
			}
		}
		// remove connections from cluster
		cluster.getMutableConnectionMap().remove(dead_connections);
	}
	return dead_connections;
}

std::list<boost::shared_ptr<components::Node> > ClusterArchitect::getRandomNodes(const int count,
		const bool allow_primary) {
	std::list<boost::shared_ptr<components::Node> > random_nodes;
	if (count > 0) {

		// Amount of random nodes to grab, allowing that some may not be suitable for deletion so will be ignored
		const unsigned int RANDOM_CHUNK_GRAB = 2 * count;
		// Count the number of grabs we've made
		unsigned int grab_count = 0;
		const unsigned int CLUSTER_PRE_SZ = cluster.getNodeMap().getSize();
		const unsigned int ui_count = static_cast<unsigned int>(count);

#ifdef CLUSTERARCHITECT_DEBUG
		int primary_nodes_skipped_count = 0;
#endif
		// Process actual deletion of nodes
		{
			while ((random_nodes.size() < ui_count) && ((RANDOM_CHUNK_GRAB * grab_count) < CLUSTER_PRE_SZ)) {
				// get a random selection of nodes
				std::vector<boost::shared_ptr<components::Node> > source_nodes =
						cluster.getMutableNodeMap().getRandomRange(RANDOM_CHUNK_GRAB);
				++grab_count;

				// forall in source_nodes
				{
					std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes =
							source_nodes.begin();
					const std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end =
							source_nodes.end();
					while ((it_source_nodes != it_source_nodes_end) && (random_nodes.size() < ui_count)) {
						// check if its attached to a fibre

						if (allow_primary == true) {
							random_nodes.push_back(*it_source_nodes);

						} else if (((*it_source_nodes)->isPrimaryInputAttachedNode() == false)
								&& ((*it_source_nodes)->isPrimaryOutputAttachedNode() == false)) {
							random_nodes.push_back(*it_source_nodes);
						}
#ifdef CLUSTERARCHITECT_DEBUG
						else {
							++primary_nodes_skipped_count;
						}
#endif
						++it_source_nodes;
					}
				}
			}

		}
#ifdef CLUSTERARCHITECT_DEBUG
		const unsigned int random_nodes_sz = random_nodes.size();
		const unsigned int nmap_sz = cluster.getNodeMap().getSize();
		const unsigned int best_node_available_count = (static_cast<unsigned int>(count) > nmap_sz) ? nmap_sz : count;
		assert(random_nodes_sz == best_node_available_count);
#endif
	}
	return random_nodes;
}

std::list<boost::shared_ptr<components::Connection> > ClusterArchitect::getRandomConnections(const int count,
		const bool allow_primary) {
	std::list<boost::shared_ptr<components::Connection> > rand_connections;

	if (count > 0) {
		std::list<boost::shared_ptr<components::Node> > rand_nodes = this->getRandomNodes(count, allow_primary);
		const int RANDOM_INPUT_COUNT = common::Maths::getRandomInteger(0, count);
		const int RANDOM_OUTPUT_COUNT = count - RANDOM_INPUT_COUNT;
		assert(RANDOM_INPUT_COUNT >= 0);
		assert(RANDOM_OUTPUT_COUNT >= 0);
		assert(RANDOM_INPUT_COUNT + RANDOM_OUTPUT_COUNT == count);

		// forall in rand_nodes
		{
			int temp_count = 0;
			std::list<boost::shared_ptr<components::Node> >::const_iterator it_rand_nodes = rand_nodes.begin();
			const std::list<boost::shared_ptr<components::Node> >::const_iterator it_rand_nodes_end = rand_nodes.end();
			while (it_rand_nodes != it_rand_nodes_end) {
				boost::shared_ptr<components::Connection> temp_conn;
				const bool get_input = (temp_count < RANDOM_INPUT_COUNT)
						&& ((*it_rand_nodes)->getConnector().getInputs().size() > 0);
				const bool get_output = (temp_count >= RANDOM_INPUT_COUNT)
						&& ((*it_rand_nodes)->getConnector().getOutputs().size() > 0);

				if (get_input == true) {
					temp_conn = (*it_rand_nodes)->getMutableConnector().getMutableInputs().begin()->second;
					++temp_count;
				} else if (get_output == true) {
					temp_conn = (*it_rand_nodes)->getMutableConnector().getMutableInputs().begin()->second;
					++temp_count;
				}
				++it_rand_nodes;
			}
		}
	}

	return rand_connections;
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

boost::shared_ptr<components::Connection> ClusterArchitect::deleteConnection(
		boost::shared_ptr<components::Connection> conn) {
	conn->disconnect();
	cluster.getMutableConnectionMap().remove(conn);
	return conn;
}

} /* namespace manipulators */
} /* namespace cryomesh */
