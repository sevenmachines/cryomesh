/*
 * Bundle.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#define BUNDLE_DEBUG

#include "Bundle.h"
#include "utilities/SequencerChannels.h"
#include <boost/uuid/uuid_io.hpp>

namespace cryomesh {

namespace structures {

Bundle::Bundle() :
	energy(0) {
	statistician = boost::shared_ptr<utilities::Statistician>(new utilities::Statistician(*this));

}

Bundle::~Bundle() {
}

void Bundle::update() {
	// update the time
	common::TimeKeeper::getTimeKeeper().update();
	// update all clusters
	clusters.update();

	// set up for future
	// update all fibres
	fibres.update();
	this->updatePrimaryInputFibres();
	//update output fibres
	this->updatePrimaryOutputFibres();

	// match all output fibres pattern channels to expected pattern channels
	this->setEnergy(this->matchOutputChannelsSum());
	// set available energy for each bundles mesh warping
	clusters.updateClusterEnergies(this->getEnergy());
	// warp bundles mesh
	clusters.warpClusterNodes();

	// TODO do things with storing output results from channels

	if (this->isDebugOn() == true) {
		this->updateStatistician();
	}
}

boost::shared_ptr<Cluster> Bundle::createCluster(int nodeSize, int nodeConnectivity) {
	boost::shared_ptr<Cluster> tempcluster(new Cluster(nodeSize, nodeConnectivity));
	clusters.add(tempcluster);
	return tempcluster;
}

boost::shared_ptr<Fibre> Bundle::connectCluster(boost::uuids::uuid inputClusterUUID,
		boost::uuids::uuid outputClusterUUID, int fibreWidth) {
	boost::shared_ptr<Cluster> inputCluster = clusters.getObjectByKey(inputClusterUUID);
	boost::shared_ptr<Cluster> outputCluster = clusters.getObjectByKey(outputClusterUUID);
	boost::shared_ptr<Fibre> newfibre;

	if (inputCluster == 0 || outputCluster == 0) {
		return newfibre;
	} else {
		newfibre = boost::shared_ptr<Fibre>(new Fibre(inputCluster, outputCluster, fibreWidth));
		inputCluster->getMutableConnector().connectOutput(newfibre);
		outputCluster->getMutableConnector().connectInput(newfibre);
	}
	fibres.add(newfibre);
	return newfibre;
}

boost::shared_ptr<Fibre> Bundle::connectCluster(boost::uuids::uuid clusterUUID, const Fibre::FibreType & type,
		int fibreWidth) {
	boost::shared_ptr<Cluster> cluster = clusters.getObjectByKey(clusterUUID);
	boost::shared_ptr<Fibre> newfibre;

	if (cluster == 0) {
		return newfibre;
	} else {
		newfibre = boost::shared_ptr<Fibre>(new Fibre(cluster, type, fibreWidth));
		if (type == Fibre::PrimaryInputFibre) {
			cluster->getMutableConnector().connectInput(newfibre);
			inputFibres.add(newfibre);

		} else if (type == Fibre::PrimaryOutputFibre) {
			// create output fibre and add to list of fibres
			cluster->getMutableConnector().connectOutput(newfibre);
			outputFibres.add(newfibre);

		} else if (type == Fibre::LoopbackFibre) {
			cluster->getMutableConnector().connectInput(newfibre);
			cluster->getMutableConnector().connectOutput(newfibre);
			fibres.add(newfibre);
		} else {
			std::cout << "Bundle::connectCluster (uuid, FibreType, int ) : "
					<< "WARNING: Ignoring improper FibreType connection call" << std::endl;
		}
	}

	//fibres.add(newfibre);
	return newfibre;
}

boost::shared_ptr<Fibre> Bundle::connectPrimaryInputCluster(boost::uuids::uuid patchanid,
		boost::uuids::uuid clusterUUID) {
	boost::shared_ptr<Fibre> newfib;
	// check for pattern channel uuid existence
	boost::shared_ptr<state::PatternChannel> patchan = realInputChannelsMap.getObjectByKey(patchanid);
	if (patchan != 0) {
		// get width from actual pattern channel
		int width = patchan->getWidth();
		// get created fibre and connect the cluster to it
		newfib = this->connectCluster(clusterUUID, Fibre::PrimaryInputFibre, width);
		realFibrePatternChannelMap[newfib->getUUID()] = patchanid;

		// make an actual pattern channel and
		// add a fibre map to the actual channel
		boost::shared_ptr<state::PatternChannel> new_actual_patchan(
				new state::PatternChannel(state::PatternChannel::Input));

		// restrict actual input channel depth  to real channel depth
		new_actual_patchan->setMaxPatternListSize(patchan->getPatternList().size());
		actualInputChannelsMap.add(new_actual_patchan);
		actualFibrePatternChannelMap[newfib->getUUID()] = new_actual_patchan->getUUID();

		//	std::cout << "Bundle::connectPrimaryInputCluster: " << "Created Fibre: " << newfib->getUUID()
		//			<< " PatternChannel: " << new_actual_patchan->getUUID() << " Set Depth: " << patchan->getPatternList().size()
		//			<< std::endl;
		//	this->printFibreMaps(std::cout)<<std::endl;

		/*
		 // connect fibre to cluster to
		 boost::shared_ptr<Cluster> cluster = clusters.getObjectByKey(clusterUUID);
		 if (cluster == 0) {
		 std::cout << "Bundle::connectPrimaryInputCluster: " << "ERROR: Cannot find cluster: " << clusterUUID
		 << std::endl;
		 return newfib;
		 } else {
		 newfib ->getMutableConnector().connectOutput(cluster);
		 // map it to pattern channel
		 fibrePatternChannelMap[newfib->getUUID()] = patchanid;
		 }*/
	} else {
		std::cout << "Bundle::connectPrimaryInputCluster: "
				<< "ERROR: PatternChannel does not exist, not creating primary input fibre. " << "'" << patchanid
				<< "'" << std::endl;
	}
	return newfib;
}

boost::shared_ptr<Fibre> Bundle::connectPrimaryOutputCluster(boost::uuids::uuid patchanid,
		boost::uuids::uuid clusterUUID) {
	boost::shared_ptr<Fibre> newfib;
	// check for pattern channel uuid existence
	boost::shared_ptr<state::PatternChannel> patchan = realOutputChannelsMap.getObjectByKey(patchanid);
	if (patchan != 0) {
		// get width from actual pattern channel
		int width = patchan->getWidth();
		// get created fibre
		newfib = this->connectCluster(clusterUUID, Fibre::PrimaryOutputFibre, width);
		realFibrePatternChannelMap[newfib->getUUID()] = patchanid;

		// make an actual pattern channel and
		// add a fibre map to the actual channel
		boost::shared_ptr<state::PatternChannel> new_actual_patchan(
				new state::PatternChannel(state::PatternChannel::Output));
		actualFibrePatternChannelMap[newfib->getUUID()] = new_actual_patchan->getUUID();
		actualOutputChannelsMap.add(new_actual_patchan);

		// restrict actual output channel depth  to real channel depth
		new_actual_patchan->setMaxPatternListSize(patchan->getPatternList().size());
		//std::cout << "Bundle::connectPrimaryOutputCluster: " << "Created Fibre: " << newfib->getUUID()
		//				<< " PatternChannel: " << new_actual_patchan->getUUID() << " Set Depth: " << patchan->getPatternList().size()
		//				<< std::endl;
		//std::cout<<"Bundle::connectPrimaryOutputCluster: "<<"Created Fibre UUID: "<<newfib->getUUIDString()<<std::endl;
		//std::cout<<"Bundle::connectPrimaryOutputCluster: "<<"Created PatternChannel UUID: "<<actual_chan->getUUID()<<std::endl;
		//std::cout<<"Bundle::connectPrimaryOutputCluster: "<<"Created PatternChannel (OLD) UUID: "<<newpatchan->getUUID()<<std::endl;
		//this->printFibreMaps(std::cout)<<std::endl;

		/*
		 // connect fibre to cluster to
		 boost::shared_ptr<Cluster> cluster = clusters.getObjectByKey(clusterUUID);
		 if (cluster == 0) {
		 std::cout << "Bundle::connectPrimaryOutputCluster: " << "ERROR: Cannot find cluster: " << clusterUUID
		 << std::endl;
		 return newfib;
		 } else {
		 newfib ->getMutableConnector().connectInput(cluster);
		 // map it to pattern channel
		 fibrePatternChannelMap[newfib->getUUID()] = patchanid;
		 }*/
	} else {
		std::cout << "Bundle::connectPrimaryOutputCluster: "
				<< "ERROR: PatternChannel does not exist, not creating primary output fibre. " << "'" << patchanid
				<< "'" << std::endl;
	}
	return newfib;
}

std::vector<boost::shared_ptr<Fibre> > Bundle::autoConnectPrimaryInputClusters(
		const std::vector<boost::uuids::uuid> & cluster_uuids) {
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryInputClusters: "<<""<<std::endl;
#endif
	std::vector<boost::shared_ptr<Cluster> > cluster_list;
	// forall in cluster__uuids
	{
		std::vector<boost::uuids::uuid>::const_iterator it_cluster_uuids = cluster_uuids.begin();
		const std::vector<boost::uuids::uuid>::const_iterator it_cluster_uuids_end = cluster_uuids.end();
		while (it_cluster_uuids != it_cluster_uuids_end) {
			boost::shared_ptr<Cluster> temp_cluster = this->getMutableClusters().getMutableObjectByKey(
					*it_cluster_uuids);
			if (temp_cluster != 0) {
				cluster_list.push_back(temp_cluster);
			} else {
				std::cout << "Bundle::autoConnectPrimaryInputClusters: " << "WARNING: Cannot find cluster by uuid: "
						<< *it_cluster_uuids << std::endl;
			}
			++it_cluster_uuids;
		}
	}
	return (this->autoConnectPrimaryInputClusters(cluster_list));
}

std::vector<boost::shared_ptr<Fibre> > Bundle::autoConnectPrimaryOutputClusters(
		const std::vector<boost::uuids::uuid> & cluster_uuids) {

	std::vector<boost::shared_ptr<Cluster> > cluster_list;
	// forall in cluster__uuids
	{
		std::vector<boost::uuids::uuid>::const_iterator it_cluster_uuids = cluster_uuids.begin();
		const std::vector<boost::uuids::uuid>::const_iterator it_cluster_uuids_end = cluster_uuids.end();
		while (it_cluster_uuids != it_cluster_uuids_end) {
			boost::shared_ptr<Cluster> temp_cluster = this->getMutableClusters().getMutableObjectByKey(
					*it_cluster_uuids);
			if (temp_cluster != 0) {
				cluster_list.push_back(temp_cluster);
			} else {
				std::cout << "Bundle::autoConnectPrimaryOutputClusters: " << "WARNING: Cannot find cluster by uuid: "
						<< *it_cluster_uuids << std::endl;
			}
			++it_cluster_uuids;
		}
	}
	return (this->autoConnectPrimaryOutputClusters(cluster_list));
}

std::vector<boost::shared_ptr<Fibre> > Bundle::autoConnectPrimaryInputClusters(
		std::vector<boost::shared_ptr<Cluster> > list) {
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryInputClusters(list): "<<""<<std::endl;
#endif
	std::vector<boost::shared_ptr<Fibre> > newfibres;

	if (list.size() > 0) {
		// get unconnected real pattern channels
		std::vector<boost::shared_ptr<state::PatternChannel> > disconnected_channels =
				this->getDisconnectedRealInputPatternChannels();
		if (disconnected_channels.size()==0){
		std::cout<<"Bundle::autoConnectPrimaryInputClusters: "<<"No disconnected real input channels"<<std::endl;
		}
		// forall in disconnected_channels
		{
			std::vector<boost::shared_ptr<Cluster> >::iterator it_list = list.begin();
			const std::vector<boost::shared_ptr<Cluster> >::const_iterator it_list_end = list.end();
			std::vector<boost::shared_ptr<state::PatternChannel> >::iterator it_disconnected_channels =
					disconnected_channels.begin();
			const std::vector<boost::shared_ptr<state::PatternChannel> >::const_iterator it_disconnected_channels_end =
					disconnected_channels.end();
			while (it_disconnected_channels != it_disconnected_channels_end) {
				// cycle round the cluster list
				if (it_list == it_list_end) {
					it_list = list.begin();
				}
				const boost::uuids::uuid channel_uuid = (*it_disconnected_channels)->getUUID();
				const boost::uuids::uuid cluster_uuid = (*it_list)->getUUID();
				boost::shared_ptr<Fibre> newfibre = this->connectPrimaryInputCluster(channel_uuid, cluster_uuid);
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryInputClusters: Connecting channel "<<channel_uuid <<" to cluster "<<cluster_uuid<<std::endl;
#endif
	newfibres.push_back(newfibre);
				++it_list;
				++it_disconnected_channels;
			}
		}
	} else {
		std::cout << "Bundle::autoConnectPrimaryInputClusters: " << "WARNING: No clusters to autoconnect" << std::endl;
	}
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryInputClusters: Created new fibres: "<<newfibres.size()<<std::endl;
#endif
	return newfibres;
}
std::vector<boost::shared_ptr<Fibre> > Bundle::autoConnectPrimaryOutputClusters(
		std::vector<boost::shared_ptr<Cluster> > list) {
	std::vector<boost::shared_ptr<Fibre> > newfibres;

	if (list.size() > 0) {
		// get unconnected real pattern channels
		std::vector<boost::shared_ptr<state::PatternChannel> > disconnected_channels =
				this->getDisconnectedRealOutputPatternChannels();
		// forall in disconnected_channels
		{
			std::vector<boost::shared_ptr<Cluster> >::iterator it_list = list.begin();
			const std::vector<boost::shared_ptr<Cluster> >::const_iterator it_list_end = list.end();
			std::vector<boost::shared_ptr<state::PatternChannel> >::iterator it_disconnected_channels =
					disconnected_channels.begin();
			const std::vector<boost::shared_ptr<state::PatternChannel> >::const_iterator it_disconnected_channels_end =
					disconnected_channels.end();
			while (it_disconnected_channels != it_disconnected_channels_end) {
				// cycle round the cluster list
				if (it_list == it_list_end) {
					it_list = list.begin();
				}
				const boost::uuids::uuid channel_uuid = (*it_disconnected_channels)->getUUID();
				const boost::uuids::uuid cluster_uuid = (*it_list)->getUUID();
				boost::shared_ptr<Fibre> newfibre = this->connectPrimaryOutputCluster(channel_uuid, cluster_uuid);
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryOutputClusters: Connecting channel "<<channel_uuid <<" to cluster "<<cluster_uuid<<std::endl;
#endif
	newfibres.push_back(newfibre);
				++it_list;
				++it_disconnected_channels;
			}
		}
	} else {
		std::cout << "Bundle::autoConnectPrimaryOutputClusters: " << "WARNING: No clusters to autoconnect" << std::endl;
	}
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::autoConnectPrimaryInputClusters: Created new fibres: "<<newfibres.size()<<std::endl;
#endif
	return newfibres;
}

std::vector<boost::shared_ptr<state::PatternChannel> > Bundle::getDisconnectedRealInputPatternChannels() {
	std::vector<boost::shared_ptr<state::PatternChannel> > disconnected_channels;

	// check if theres a mapping of a fibre to the pattern channel, if not, return it
	std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > & channel_map =
			realInputChannelsMap.getMutableCollection();
	// forall in channel_map
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::iterator it_channel_map =
				channel_map.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_channel_map_end = channel_map.end();
		while (it_channel_map != it_channel_map_end) {
			boost::shared_ptr<Fibre> found_fibre = this->getPrimaryInputFibreByRealChannel(
					it_channel_map->second->getUUID());
			if (found_fibre == 0) {
				disconnected_channels.push_back(it_channel_map->second);
			}
			++it_channel_map;
		}
	}
#ifdef BUNDLE_DEBUG
	std::cout<<"Bundle::getDisconnectedRealInputPatternChannels: "<<""<<std::endl;
#endif
	return disconnected_channels;
}
std::vector<boost::shared_ptr<state::PatternChannel> > Bundle::getDisconnectedRealOutputPatternChannels() {
	std::vector<boost::shared_ptr<state::PatternChannel> > disconnected_channels;

	// check if theres a mapping of a fibre to the pattern channel, if not, return it
	std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > & channel_map =
			realOutputChannelsMap.getMutableCollection();
	// forall in channel_map
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::iterator it_channel_map =
				channel_map.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_channel_map_end = channel_map.end();
		while (it_channel_map != it_channel_map_end) {
			boost::shared_ptr<Fibre> found_fibre = this->getPrimaryOutputFibreByRealChannel(
					it_channel_map->second->getUUID());
			if (found_fibre == 0) {
				disconnected_channels.push_back(it_channel_map->second);
			}
			++it_channel_map;
		}
	}
	return disconnected_channels;
}

boost::shared_ptr<Fibre> Bundle::connectLoopbackCluster(boost::uuids::uuid clusterUUID, int fibreWidth) {
	return this->connectCluster(clusterUUID, Fibre::LoopbackFibre, fibreWidth);
}

void Bundle::loadChannels(const std::string & ifstr) {
	std::cout << "Bundle::loadChannels: " << "" << std::endl;
	utilities::SequencerChannels seqchans;
	seqchans.readSequences(ifstr, realInputChannelsMap, realOutputChannelsMap);
	// get list of input channels
	//	inputChannelsMap = seqchans.getInputChannelsMap();

	// ditto for output channels
	//outputChannelsMap = seqchans.getOutputChannelsMap();
	//this->printChannels(std::cout);
}

const ClusterMap & Bundle::getClusters() const {
	return clusters;
}

void Bundle::enableDebug(bool b){
	this->enableDebugClusters(b);
	this->enableDebugFibres(b);
}

void Bundle::enableDebugClusters(bool b) {
	clusters.enableDebug(b);
}
void Bundle::enableDebugFibres(bool b) {
	fibres.enableDebug(b);
}

ClusterMap & Bundle::getMutableClusters() {
	return clusters;
}
const FibreMap & Bundle::getFibres() const {
	return fibres;
}

const FibreMap & Bundle::getInputFibres() const {
	return inputFibres;
}

FibreMap & Bundle::getMutableInputFibres() {
	return inputFibres;
}

const FibreMap & Bundle::getOutputFibres() const {
	return outputFibres;
}

FibreMap & Bundle::getMutableOutputFibres() {
	return outputFibres;
}

const state::PatternChannelMap & Bundle::getRealInputChannelsMap() const {
	return realInputChannelsMap;
}
const state::PatternChannelMap & Bundle::getRealOutputChannelsMap() const {
	return realOutputChannelsMap;
}
const state::PatternChannelMap & Bundle::getActualInputChannelsMap() const {
	return actualInputChannelsMap;
}
const state::PatternChannelMap & Bundle::getActualOutputChannelsMap() const {
	return actualOutputChannelsMap;
}

const std::map<boost::uuids::uuid, boost::uuids::uuid> & Bundle::getRealFibrePatternChannelMap() const {
	return realFibrePatternChannelMap;
}
const std::map<boost::uuids::uuid, boost::uuids::uuid> & Bundle::getActualFibrePatternChannelMap() const {
	return actualFibrePatternChannelMap;
}
const boost::shared_ptr<utilities::Statistician> Bundle::getStatistician() const {
	return statistician;
}

double Bundle::getEnergy() const {
	return energy;
}
boost::shared_ptr<utilities::Statistician> Bundle::getMutableStatistician() {
	return statistician;
}

bool Bundle::checkFibreStructure() const {
	bool success = true;
	const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & all_fibres = this->getFibres().getCollection();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & all_input_fibres =
			this->getInputFibres().getCollection();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & all_output_fibres =
			this->getOutputFibres().getCollection();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters =
			this->getClusters().getCollection();

	// check input fibres are all connected
	// forall in all_input_fibres
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_input_fibres =
				all_input_fibres.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_input_fibres_end =
				all_input_fibres.end();
		while (it_all_input_fibres != it_all_input_fibres_end) {
			boost::shared_ptr<Fibre> fibre = it_all_input_fibres->second;
			std::pair<int, int> concount = fibre->countConnections(all_clusters);
			if (concount.first != 0 || concount.second != 1) {
				success = false;
				std::cout << "Bundle::checkStructure: " << "WARNING: Input fibre has bad connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			} else {
				std::cout << "Bundle::checkStructure: " << "PASS: Input fibre has good connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			}
			++it_all_input_fibres;
		}
	}

	// forall in all_output_fibres
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_output_fibres =
				all_output_fibres.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_output_fibres_end =
				all_output_fibres.end();
		while (it_all_output_fibres != it_all_output_fibres_end) {
			boost::shared_ptr<Fibre> fibre = it_all_output_fibres->second;
			std::pair<int, int> concount = fibre->countConnections(all_clusters);
			if (concount.first != 1 || concount.second != 0) {
				success = false;
				std::cout << "Bundle::checkStructure: " << "WARNING: Output fibre has bad connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			} else {
				std::cout << "Bundle::checkStructure: " << "PASS: Output fibre has good connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			}
			++it_all_output_fibres;
		}
	}

	// forall in all_fibres
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_fibres = all_fibres.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_fibres_end =
				all_fibres.end();
		while (it_all_fibres != it_all_fibres_end) {
			boost::shared_ptr<Fibre> fibre = it_all_fibres->second;
			std::pair<int, int> concount = fibre->countConnections(all_clusters);
			if (concount.first != 1 || concount.second != 1) {
				success = false;
				std::cout << "Bundle::checkStructure: " << "WARNING: Intermediate fibre has bad connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			} else {
				std::cout << "Bundle::checkStructure: " << "PASS: Intermediate fibre has good connection count ("
						<< concount.first << ", " << concount.second << ")" << " '" << fibre->getUUIDString() << "'"
						<< std::endl;
			}
			++it_all_fibres;
		}
	}
	return success;
}

bool Bundle::checkChannelStructure() const {
	return false;
}
bool Bundle::checkStructure() const {
	bool success = true;
	success = success && this->checkFibreStructure();
	success = success && this->checkChannelStructure();
	return success;
}

void Bundle::updatePrimaryInputFibres() {
	const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > & channels =
			realInputChannelsMap.getCollection();

	// forall in channels
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_channels =
				channels.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_channels_end =
				channels.end();
		while (it_channels != it_channels_end) {
			// get the next pattern
			boost::shared_ptr<state::Pattern> pattern = it_channels->second->nextPattern();
			// get the related fibre
			boost::shared_ptr<Fibre> temp_fib = this->getPrimaryInputFibreByRealChannel(it_channels->second->getUUID());
			if (temp_fib != 0) {
				// find the channel
				boost::shared_ptr<state::PatternChannel> patchan = this->getActualPrimaryInputChannelByFibre(
						temp_fib->getUUID());
				if (patchan != 0) {
					// get pattern from fibre and add it to output channel
					boost::shared_ptr<state::Pattern> temp_pattern = temp_fib->getInputNodesPattern();
					patchan->addPattern(temp_pattern);
					std::cout << "Bundle::updatePrimaryInputFibres: uuid: " << temp_fib->getUUID() << " pattern ("
							<< patchan->getPatternMap().size() << "): " << temp_pattern->getString() << std::endl;
				} else {
					std::cout << "Bundle::updatePrimaryInputFibres: "
							<< "WARNING: Ignoring primary input fibre since cannot find corresponding channel: "
							<< temp_fib->getUUID() << std::endl;
				}

				// apply firing pattern to fibre
				temp_fib->trigger(*pattern);
				std::cout << "Bundle::updatePrimaryInputFibres: " << pattern->getString() << std::endl;
			} else {
				std::cout << "Bundle::updatePrimaryInputFibres: "
						<< "WARNING: getPrimaryInputFibreByChannel returned null, ignoring..." << std::endl;
			}
			++it_channels;
		}
	}

	// actual update the input fibres
	inputFibres.update();
}

void Bundle::updatePrimaryOutputFibres() {
	// forall in outputFibres
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_outputFibres = outputFibres.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_outputFibres_end =
				outputFibres.end();
		while (it_outputFibres != it_outputFibres_end) {
			// find the channel
			boost::shared_ptr<state::PatternChannel> patchan = this->getActualPrimaryOutputChannelByFibre(
					it_outputFibres->first);
			if (patchan != 0) {
				// get pattern from fibre and add it to output channel
				boost::shared_ptr<state::Pattern> temp_pattern = it_outputFibres->second->getInputNodesPattern();
				patchan->addPattern(temp_pattern);
				std::cout << "Bundle::updatePrimaryOutputFibres: uuid: " << it_outputFibres->first << " pattern ("
						<< patchan->getPatternMap().size() << "): " << temp_pattern->getString() << std::endl;
			} else {
				std::cout << "Bundle::updatePrimaryOutputFibres: "
						<< "WARNING: Ignoring primary output fibre since cannot find corresponding channel: "
						<< it_outputFibres->first << std::endl;
			}
			++it_outputFibres;
		}
		//	std::cout << printFibreMaps(std::cout) << std::endl;
	}

	// and actually update fibres, not actually required at the moment since we're using the input nodes as the end result
	outputFibres.update();
}

double Bundle::matchOutputChannelsSum() const {
	// just sum the total of all output channels
	double sum = 0;
	const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & all_output_fibres =
			this->getOutputFibres().getCollection();
	// forall in all_output_fibres
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_output_fibres =
				all_output_fibres.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_output_fibres_end =
				all_output_fibres.end();
		while (it_all_output_fibres != it_all_output_fibres_end) {
			boost::shared_ptr<state::PatternChannel> real_out_channel = this->getRealPrimaryOutputChannelByFibre(
					it_all_output_fibres->second->getUUID());
			boost::shared_ptr<state::PatternChannel> actual_out_channel = this->getActualPrimaryOutputChannelByFibre(
					it_all_output_fibres->second->getUUID());
			if (real_out_channel != 0) {
				if (actual_out_channel != 0) {
					double channel_match = actual_out_channel->matchGlobally(*real_out_channel);
					sum += channel_match;
				} else {
					std::cout << "Bundle::matchOutputChannelsSum: " << "ERROR: Actual Output Channel is Null..."
							<< std::endl;
				}
			} else {
				std::cout << "Bundle::matchOutputChannelsSum: " << "ERROR: Real Output Channel is Null..." << std::endl;
			}

			++it_all_output_fibres;
		}
	}
	return sum;
}

void Bundle::setEnergy(double d) {
	this->energy = d;
}

void Bundle::updateStatistician() {
	if (this->isDebugOn() == true) {
		std::cout << "Bundle::updateStatistician: " << "" << std::endl;
		if (this->statistician == 0) {
			statistician = boost::shared_ptr<utilities::Statistician>(new utilities::Statistician(*this));
		}
		statistician->update();
	} else {
		std::cout << "Bundle::updateStatistician: "
				<< "WARNING: Ignoring update statistician, debugging isnt enabled. " << std::endl;
	}
}

const boost::shared_ptr<state::PatternChannel> Bundle::getRealPrimaryInputChannelByFibre(
		const boost::uuids::uuid fibre_uuid) const {
	return this->getPrimaryChannelByFibre(fibre_uuid, realInputChannelsMap, realFibrePatternChannelMap);
}
const boost::shared_ptr<state::PatternChannel> Bundle::getRealPrimaryOutputChannelByFibre(
		const boost::uuids::uuid fibre_uuid) const {
	return this->getPrimaryChannelByFibre(fibre_uuid, realOutputChannelsMap, realFibrePatternChannelMap);
}
const boost::shared_ptr<state::PatternChannel> Bundle::getActualPrimaryInputChannelByFibre(
		const boost::uuids::uuid fibre_uuid) const {
	return this->getPrimaryChannelByFibre(fibre_uuid, actualInputChannelsMap, actualFibrePatternChannelMap);
}
const boost::shared_ptr<state::PatternChannel> Bundle::getActualPrimaryOutputChannelByFibre(
		const boost::uuids::uuid fibre_uuid) const {
	return this->getPrimaryChannelByFibre(fibre_uuid, actualOutputChannelsMap, actualFibrePatternChannelMap);
}

const boost::shared_ptr<Fibre> Bundle::getPrimaryInputFibreByRealChannel(const boost::uuids::uuid pattern_channel_uuid) const {
	return this->getPrimaryFibreByChannel(pattern_channel_uuid, inputFibres, realFibrePatternChannelMap);
}
const boost::shared_ptr<Fibre> Bundle::getPrimaryOutputFibreByRealChannel(const boost::uuids::uuid pattern_channel_uuid) const {
	return this->getPrimaryFibreByChannel(pattern_channel_uuid, outputFibres, realFibrePatternChannelMap);
}
const boost::shared_ptr<Fibre> Bundle::getPrimaryInputFibreByActualChannel(
		const boost::uuids::uuid pattern_channel_uuid) const {
	return this->getPrimaryFibreByChannel(pattern_channel_uuid, inputFibres, actualFibrePatternChannelMap);
}
const boost::shared_ptr<Fibre> Bundle::getPrimaryOutputFibreByActualChannel(
		const boost::uuids::uuid pattern_channel_uuid) const {
	return this->getPrimaryFibreByChannel(pattern_channel_uuid, outputFibres, actualFibrePatternChannelMap);
}
const boost::shared_ptr<Fibre> Bundle::getPrimaryFibreByChannel(const boost::uuids::uuid id, const FibreMap & map,
		const std::map<boost::uuids::uuid, boost::uuids::uuid> & fibrepattern_channelmap) const {
	boost::uuids::uuid fibre_id;
	boost::shared_ptr<Fibre> found_fibre;
	// forall in fibrePatternChannelMap
	{
		bool found = false;
		std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibrePatternChannelMap =
				fibrepattern_channelmap.begin();
		const std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibrePatternChannelMap_end =
				fibrepattern_channelmap.end();
		while (it_fibrePatternChannelMap != it_fibrePatternChannelMap_end && found != true) {
			if (it_fibrePatternChannelMap->second == id) {
				fibre_id = it_fibrePatternChannelMap->first;
				found = true;
			}
			++it_fibrePatternChannelMap;
		}
	}

	if (fibre_id != boost::uuids::nil_uuid()) {
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_fibre_found =
				map.getCollection().find(fibre_id);
		if (it_fibre_found != map.getCollection().end()) {
			found_fibre = it_fibre_found->second;
		} else {
			std::cout << "Bundle::getPrimaryFibreByChannel: " << "WARNING: Cannot find primary fibre in map: "
					<< fibre_id << std::endl;
			std::cout << "{";
			const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & temp_map = map.getCollection();
			// forall in temp_map
			{
				std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_temp_map = temp_map.begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_temp_map_end =
						temp_map.end();
				while (it_temp_map != it_temp_map_end) {
					std::cout << " " << it_temp_map->first;
					++it_temp_map;
					if (it_temp_map != it_temp_map_end) {
						std::cout << ", ";
					}
				}
			}
			std::cout << " }" << std::endl;
		}
	} else {
		std::cout << "Bundle::getPrimaryFibreByChannel: " << "WARNING: Cannot find mapped pattern channel: " << id
				<< std::endl;
	}
	return found_fibre;
}
const boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryChannelByFibre(const boost::uuids::uuid id,
		const state::PatternChannelMap & map,
		const std::map<boost::uuids::uuid, boost::uuids::uuid> & fibrepattern_channelmap) const {
	boost::uuids::uuid pattern_channel_id;
	boost::shared_ptr<state::PatternChannel> found_channel;

	const std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_found = fibrepattern_channelmap.find(id);
	if (it_found != fibrepattern_channelmap.end()) {
		pattern_channel_id = it_found->second;

		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_patchan_found =
				map.getCollection().find(pattern_channel_id);
		if (it_patchan_found != map.getCollection().end()) {
			found_channel = it_patchan_found->second;
		} else {
			std::cout << "Bundle::getPrimaryChannelByFibre: " << "WARNING: Cannot find pattern channel in map: "
					<< pattern_channel_id << std::endl;
			//	std::cout << printChannels(std::cout) << std::endl;
		}
	} else {
		std::cout << "Bundle::getPrimaryChannelByFibre: " << "WARNING: Cannot find mapped fibre: " << id << std::endl;
	}

	return found_channel;
}

template<class T>
std::ostream & Bundle::printSearch(std::ostream & os, const boost::uuids::uuid & uuid,
		const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & map) const {
	os << uuid << " : " << "{";
	// forall in temp_map
	{
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_temp_map = map.begin();
		const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_temp_map_end = map.end();
		while (it_temp_map != it_temp_map_end) {
			os << " (" << it_temp_map->first
			//<< " : "<<it_temp_map->second->getUUIDString()
					<< ") ";
			++it_temp_map;
			if (it_temp_map != it_temp_map_end) {
				os << ", ";
			}
		}
	}

	os << " }" << std::endl;
	return os;
}

std::ostream& Bundle::print(std::ostream & os, const common::Loggable::LoggingDepth depth) const {
	std::string tab;
	if (depth == SUMMARY) {
		os << std::endl << "SUMMARY **************************************" << std::endl << std::endl;
	} else if (depth == MAX) {
		os << std::endl << "MAX *******************************************" << std::endl << std::endl;
	}
	if (depth == SUMMARY || depth == MAX) {

		// bundle general details
		os << "Bundle: " << "cycle: " << common::TimeKeeper::getTimeKeeper().getCycle() << " clusters:"
				<< this->getClusters().getSize() << " fibres:" << this->getFibres().getSize() << std::endl;
	}
	if (depth == MAX) {

		// Clustermap
		os << this->getClusters() << std::endl;
		// fibremap
		os << this->getFibres() << std::endl;
		// connecting fibres patterns

		os << this->getInputFibres() << std::endl;
		os << this->getOutputFibres() << std::endl;

		this->printFibreMaps(os) << std::endl << std::endl;
		this->printChannels(os) << std::endl << std::endl;

	}
	os << std::endl << "***********************************************" << std::endl << std::endl;
	return os;
}

std::ostream& Bundle::printChannels(std::ostream & os) const {
	std::cout << " Bundle::printChannels: " << "" << std::endl;

	// forall in in_channels
	{
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > in_channels =
				realInputChannelsMap.getCollection();
		os << "Real Input Channels -> {";
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_in_channels =
				in_channels.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_in_channels_end = in_channels.end();
		while (it_in_channels != it_in_channels_end) {
			os << " (" << it_in_channels->first << " : " << it_in_channels->second->getRefID() << ")";
			++it_in_channels;
			if (it_in_channels != it_in_channels_end) {
				os << "," << std::endl;
			}
		}
		os << "}" << std::endl;
	}

	// forall in out_channels
	{
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > out_channels =
				realOutputChannelsMap.getCollection();
		os << "Real Output Channels -> {";
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_out_channels =
				out_channels.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_out_channels_end = out_channels.end();
		while (it_out_channels != it_out_channels_end) {
			os << " (" << it_out_channels->first << " : " << it_out_channels->second->getRefID() << ")";
			++it_out_channels;
			if (it_out_channels != it_out_channels_end) {
				os << "," << std::endl;
			}
		}
		os << "}" << std::endl;
	}

	// forall in in_channels
	{
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > actual_in_channels =
				actualInputChannelsMap.getCollection();
		os << "Actual Input Channels -> {";
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_in_channels =
				actual_in_channels.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_in_channels_end = actual_in_channels.end();
		while (it_in_channels != it_in_channels_end) {
			os << " (" << it_in_channels->first << " : " << it_in_channels->second->getRefID() << ")";
			++it_in_channels;
			if (it_in_channels != it_in_channels_end) {
				os << "," << std::endl;
			}
		}
		os << "}" << std::endl;
	}

	// forall in out_channels
	{
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > actual_out_channels =
				actualOutputChannelsMap.getCollection();
		os << "Actual Output Channels -> {";
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_out_channels =
				actual_out_channels.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
				it_out_channels_end = actual_out_channels.end();
		while (it_out_channels != it_out_channels_end) {
			os << " (" << it_out_channels->first << " : " << it_out_channels->second->getRefID() << ")";
			++it_out_channels;
			if (it_out_channels != it_out_channels_end) {
				os << "," << std::endl;
			}
		}
		os << "}" << std::endl;
	}

	return os;
}

std::ostream & Bundle::printFibreMaps(std::ostream & os) const {

	std::cout << "Bundle::printFibreMaps: " << "RealFibrePatternChannelMap" << std::endl;
	printFibreMap(os, this->getRealFibrePatternChannelMap());
	std::cout << "Bundle::printFibreMaps: " << "ActualFibrePatternChannelMap" << std::endl;
	printFibreMap(os, this->getActualFibrePatternChannelMap());
	return os;
}

std::ostream & Bundle::printFibreMap(std::ostream & os,
		const std::map<boost::uuids::uuid, boost::uuids::uuid> & fibre_map) const {
	// forall in fibre_map
	{
		std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibre_map = fibre_map.begin();
		const std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibre_map_end = fibre_map.end();
		while (it_fibre_map != it_fibre_map_end) {
			os << it_fibre_map->first << " : " << it_fibre_map->second << std::endl;
			++it_fibre_map;
		}
	}
	return os;
}

std::ostream& operator<<(std::ostream & os, const Bundle & obj) {
	std::cout << "Bundle::operator <<: " << "" << std::endl;
	return obj.print(os, common::Loggable::MAX);
}

}//NAMESPACE

}//NAMESPACE
