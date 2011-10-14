/*
 * ClusterArchitect.cpp
 *
 *  Created on: 30 Sep 2011
 *      Author: niall
 */

//#define CLUSTERARCHITECT_DEBUG
#include "ClusterArchitect.h"
#include "ClusterAnalyserBasic.h"
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
const unsigned int ClusterArchitect::DEFAULT_HISTORY_STEPPING_FACTOR = 10;

ClusterArchitect::ClusterArchitect(structures::Cluster & clus, const  int max_history_sz, const  int history_stepping_factor) :
		cluster(clus), histories(), historySteppingFactor(history_stepping_factor), clusterAnalyser(
				new ClusterAnalyserBasic), currentClusterAnalysisData(), maxHistorySize(max_history_sz) {
	histories[1] = std::list<ClusterAnalysisData>();
}

ClusterArchitect::~ClusterArchitect() {
}

void ClusterArchitect::runAnalysis() {
	// update history
	ClusterAnalysisData cad = clusterAnalyser->analyseCluster(cluster, histories);
	this->addHistoryEntry(cad);

	this->destroyRandomNodes(cad.getNodesToDestroy());
	this->createRandomNodes(cad.getNodesToCreate());
	this->destroyRandomConnections(cad.getConnectionsToDestroy());
	this->createRandomConnections(cad.getConnectionsToCreate());
}

std::set<boost::shared_ptr<cryomesh::components::Node> > ClusterArchitect::createRandomNodes(int count,
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
	unsigned int pre_nodes_sz = cluster.getNodeMap().getSize();
#endif

	// new nodes that will be created
	std::set<boost::shared_ptr<cryomesh::components::Node> > new_nodes;

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
			new_nodes.insert(tempnode);
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
					std::set<boost::shared_ptr<cryomesh::components::Node> >::iterator it_new_nodes = new_nodes.begin();
					const std::set<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
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
							if ((self_connect || not_same) && (!evenly_distribute || node_has_capacity)) {
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
					std::set<boost::shared_ptr<cryomesh::components::Node> >::iterator it_new_nodes = new_nodes.begin();
					const std::set<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
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
							if ((self_connect || not_same) && (!evenly_distribute || node_has_capacity)) {
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
					std::set<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes =
					new_nodes.begin();
					const std::set<boost::shared_ptr<cryomesh::components::Node> >::const_iterator it_new_nodes_end =
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

std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > ClusterArchitect::createRandomConnections(
		int count) {

	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > new_connections;

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
				new_connections[tempcon->getUUID()] = (tempcon);
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

std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > ClusterArchitect::destroyRandomNodes(int count) {
	std::cout << "ClusterArchitect::destroyRandomNodes: " << "count: " << count << std::endl;
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > dead_nodes;

	if (count > 0) {

		// get a random selection of nodes
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > source_nodes = this->getRandomNodes(count,
				false);

		// forall in source_nodes
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_source_nodes =
					source_nodes.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end =
					source_nodes.end();
			while (it_source_nodes != it_source_nodes_end) {
				// get inputs and delete them from cluster
				cluster.getMutableConnectionMap().remove(it_source_nodes->second->getConnector().getInputsUUID());
				// now disconnect node
				it_source_nodes->second->destroyAllConnections();
				dead_nodes.insert(*it_source_nodes);
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

std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > ClusterArchitect::destroyRandomConnections(
		int count) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > dead_connections;
	if (count > 0) {
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > rand_conns =
				this->getRandomConnections(count, false);
#ifdef CLUSTERARCHITECT_DEBUG
		std::cout << "ClusterArchitect::destroyRandomConnections: " << "count: " << count << " available: "
		<< rand_conns.size() << std::endl;
#endif
		// forall in rand_conns
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::iterator it_rand_conns =
					rand_conns.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_rand_conns_end =
					rand_conns.end();
			while (it_rand_conns != it_rand_conns_end) {
				// delete
				it_rand_conns->second->disconnect();
				dead_connections.insert(*it_rand_conns);
				++it_rand_conns;
			}
		}
		// remove connections from cluster
#ifdef CLUSTERARCHITECT_DEBUG
		std::cout << "ClusterArchitect::destroyRandomConnections: delete " << dead_connections.size() << " of "
		<< cluster.getConnectionMap().getSize() << "in cluster" << std::endl;
// forall in dead_connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_dead_connections =
			dead_connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_dead_connections_end =
			dead_connections.end();
			while (it_dead_connections != it_dead_connections_end) {
				const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_found =
				cluster.getConnections().find(it_dead_connections->first);
				assert(it_found != cluster.getConnections().end());
				++it_dead_connections;
			}
		}
#endif
		cluster.getMutableConnectionMap().remove(dead_connections);
	}
	return dead_connections;
}

std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > ClusterArchitect::getRandomNodes(const int count,
		const bool allow_primary) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > random_nodes;
	if (count > 0) {

		unsigned int best_available_count;
		if (allow_primary == true) {
			const unsigned int available_nodes = cluster.getNodeMap().getSize();
			const unsigned int ui_count = static_cast<unsigned int>(count);
			best_available_count = (ui_count < available_nodes) ? count : available_nodes;
		} else {
			const unsigned int all_nodes = cluster.getNodeMap().getSize();
			const unsigned int pin_nodes = cluster.getNodeMap().getAllPrimaryInputNodes().size();
			const unsigned int pout_nodes = cluster.getNodeMap().getAllPrimaryOutputNodes().size();

			const unsigned int available_nodes = all_nodes - pin_nodes - pout_nodes;
			const unsigned int ui_count = static_cast<unsigned int>(count);
			best_available_count = (ui_count < available_nodes) ? count : available_nodes;
		}

		// Amount of random nodes to grab, allowing that some may not be suitableso will be ignored
		const unsigned int RANDOM_CHUNK_GRAB = best_available_count + 0.1 * best_available_count;

		// Process actual collection of nodes
		{
			while (random_nodes.size() < best_available_count) {
				// get a random selection of nodes
				std::vector<boost::shared_ptr<components::Node> > source_nodes =
						cluster.getMutableNodeMap().getRandomRange(RANDOM_CHUNK_GRAB);

				// forall in source_nodes
				{
					std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes =
							source_nodes.begin();
					const std::vector<boost::shared_ptr<components::Node> >::const_iterator it_source_nodes_end =
							source_nodes.end();
					while ((it_source_nodes != it_source_nodes_end) && (random_nodes.size() < best_available_count)) {
						// check if its attached to a fibre

						if (allow_primary == true) {
							random_nodes[(*it_source_nodes)->getUUID()] = (*it_source_nodes);

						} else if (((*it_source_nodes)->isPrimaryInputAttachedNode() == false)
								&& ((*it_source_nodes)->isPrimaryOutputAttachedNode() == false)) {
							random_nodes[(*it_source_nodes)->getUUID()] = (*it_source_nodes);
						}
						++it_source_nodes;
					}
				}
			}

		}
	}
	return random_nodes;
}

std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > ClusterArchitect::getRandomConnections(
		const int count, const bool allow_primary) {
	std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > random_selected;
	if (count > 0) {

		unsigned int best_available_count;
		if (allow_primary == true) {
			const unsigned int available_nodes = cluster.getConnectionMap().getSize();
			const unsigned int ui_count = static_cast<unsigned int>(count);
			best_available_count = (ui_count < available_nodes) ? count : available_nodes;
		} else {
			const unsigned int all_nodes = cluster.getConnectionMap().getSize();
			const unsigned int pin_nodes = cluster.getConnectionMap().getAllPrimaryInputConnections().size();
			const unsigned int pout_nodes = cluster.getConnectionMap().getAllPrimaryOutputConnections().size();

			const unsigned int available_nodes = all_nodes - pin_nodes - pout_nodes;
			const unsigned int ui_count = static_cast<unsigned int>(count);
			best_available_count = (ui_count < available_nodes) ? count : available_nodes;
		}

		// Amount of random nodes to grab, allowing that some may not be suitable so will be ignored
		const unsigned int RANDOM_CHUNK_GRAB = best_available_count + 0.1 * best_available_count;

		{
			while (random_selected.size() < best_available_count) {
				// get a random selection of connections
				std::vector<boost::shared_ptr<components::Connection> > sources =
						cluster.getMutableConnectionMap().getRandomRange(RANDOM_CHUNK_GRAB);

				// forall in source_nodes
				{
					std::vector<boost::shared_ptr<components::Connection> >::const_iterator it_sources =
							sources.begin();
					const std::vector<boost::shared_ptr<components::Connection> >::const_iterator it_sources_end =
							sources.end();
					while ((it_sources != it_sources_end) && (random_selected.size() < best_available_count)) {
						// check if its attached to a fibre

						if (allow_primary == true) {
							random_selected[(*it_sources)->getUUID()] = (*it_sources);

						} else if (((*it_sources)->isPrimaryInputConnection() == false)
								&& ((*it_sources)->isPrimaryOutputConnection() == false)) {
							random_selected[(*it_sources)->getUUID()] = (*it_sources);
						}
						++it_sources;
					}
				}
			}

		}
	}
	return random_selected;
}

const std::map<int, std::list<ClusterAnalysisData> > & ClusterArchitect::getHistories() const {
	return histories;
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

	std::list<ClusterAnalysisData> & onestep_history = histories[1];
	onestep_history.push_back(entry);

	while (onestep_history.size() > max_sz) {
		onestep_history.pop_front();
	}
	currentClusterAnalysisData = entry;

	int present_step = 1;
	bool do_history_calc = true;

	while (do_history_calc == true) {
// do recursive histories
		int next_step = present_step * historySteppingFactor;

		std::list<ClusterAnalysisData> & present_history = histories[present_step];
		std::list<ClusterAnalysisData> & next_history = histories[next_step];

		// do calculate history step average if,
		// - present history is the right size
		// - either theres no next_history entries or the present_history and next_histories last entry cycles have a diff
		bool enough_entries_since_last_calc = (next_history.size() == 0)
				|| ((present_history.rbegin()->getClusterRangeEnergy().endCycle - next_history.rbegin()->getClusterRangeEnergy().endCycle)
						> (present_step * historySteppingFactor));
		do_history_calc = (present_history.size() == max_sz) && enough_entries_since_last_calc;

		if (do_history_calc) {
			ClusterAnalysisData data = clusterAnalyser->calculateRangeEnergies(present_history);
			next_history.push_back(data);
		}

		present_step = next_step;
	}
}

void ClusterArchitect::splitHistoryByValue(const std::list<ClusterAnalysisData> & history, double db, int countback,
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
			if (it_history->getClusterRangeEnergy().energy > db) {
				above[it_history->getClusterRangeEnergy().endCycle] = *it_history;
			} else {
				below[it_history->getClusterRangeEnergy().endCycle] = *it_history;
			}
			--countback;
			++it_history;
		}
	}
	assert(below.size() + above.size() == history.size());
}

std::vector<ClusterAnalysisData> ClusterArchitect::getHistoryEntriesInRange(
		const std::list<ClusterAnalysisData> & history, double min_db, double max_db, int count) const {
	std::map<common::Cycle, ClusterAnalysisData> above_max;
	std::map<common::Cycle, ClusterAnalysisData> below_max;
	std::map<common::Cycle, ClusterAnalysisData> above_min;
	std::map<common::Cycle, ClusterAnalysisData> below_min;
	this->splitHistoryByValue(history, min_db, count, below_min, above_min);
	this->splitHistoryByValue(history, max_db, count, below_max, above_max);

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
