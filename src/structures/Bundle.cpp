/*
 * Bundle.cpp
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#include "Bundle.h"
#include "utilities/SequencerChannels.h"
#include <boost/uuid/uuid_io.hpp>

namespace cryomesh {

namespace structures {

Bundle::Bundle() {

}

Bundle::~Bundle() {
}

void Bundle::update() {
	// update the time
	common::TimeKeeper::getTimeKeeper().update();
	//update input primaries
	this->updatePrimaryInputFibres();

	// update all fibres
	fibres.update();
	// update all clusters
	clusters.update();

	//update output fibres
	this->updatePrimaryOutputFibres();

	// TODO do evolution things

	// TODO do things with storing output results
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
	boost::shared_ptr<state::PatternChannel> patchan = inputChannelsMap.getObjectByKey(patchanid);
	if (patchan != 0) {
		// get width from actual pattern channel
		int width = patchan->getWidth();
		// get created fibre and connect the cluster to it
		newfib = this->connectCluster(clusterUUID, Fibre::PrimaryInputFibre, width);
		fibrePatternChannelMap[newfib->getUUID()] = patchanid;

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

boost::shared_ptr<Fibre> Bundle::connectPrimaryInputCluster(boost::uuids::uuid clusterUUID, int fibreWidth) {
	return this->connectCluster(clusterUUID, Fibre::PrimaryInputFibre, fibreWidth);
}

boost::shared_ptr<Fibre> Bundle::connectPrimaryOutputCluster(boost::uuids::uuid patchanid,
		boost::uuids::uuid clusterUUID) {
	boost::shared_ptr<Fibre> newfib;
	// check for pattern channel uuid existence
	boost::shared_ptr<state::PatternChannel> patchan = outputChannelsMap.getObjectByKey(patchanid);
	if (patchan != 0) {
		// get width from actual pattern channel
		int width = patchan->getWidth();
		// get created fibre
		newfib = this->connectCluster(clusterUUID, Fibre::PrimaryOutputFibre, width);
		fibrePatternChannelMap[newfib->getUUID()] = patchanid;

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

boost::shared_ptr<Fibre> Bundle::connectPrimaryOutputCluster(boost::uuids::uuid clusterUUID, int fibreWidth) {
	return this->connectCluster(clusterUUID, Fibre::PrimaryOutputFibre, fibreWidth);
}

boost::shared_ptr<Fibre> Bundle::connectLoopbackCluster(boost::uuids::uuid clusterUUID, int fibreWidth) {
	return this->connectCluster(clusterUUID, Fibre::LoopbackFibre, fibreWidth);
}

void Bundle::loadChannels(const std::string & ifstr) {
	utilities::SequencerChannels seqchans;
	seqchans.readSequences(ifstr);

	// get list of input channels
	inputChannelsMap = seqchans.getInputChannelsMap();

	// ditto for output channels
	outputChannelsMap = seqchans.getOutputChannelsMap();
}

const ClusterMap & Bundle::getClusters() const {
	return clusters;
}

const FibreMap & Bundle::getFibres() const {
	return fibres;
}

const FibreMap & Bundle::getInputFibres() const {
	return inputFibres;
}
const FibreMap & Bundle::getOutputFibres() const {
	return outputFibres;
}

const state::PatternChannelMap & Bundle::getInputChannelsMap() const {
	return inputChannelsMap;
}
const state::PatternChannelMap & Bundle::getOutputChannelsMap() const {
	return outputChannelsMap;
}
const std::map<boost::uuids::uuid, boost::uuids::uuid> & Bundle::getFibrePatternChannelMap() const {
	return fibrePatternChannelMap;
}

std::ostream & operator<<(std::ostream & os, const Bundle & bundle) {
	std::string tab;
	// bundle general details
	os << "Bundle: " << "clusters:" << bundle.getClusters().getSize() << " fibres:" << bundle.getFibres().getSize()
			<< std::endl;

	// Clustermap
	os << bundle.getClusters() << std::endl;
	// fibremap
	os << bundle.getFibres() << std::endl;
	// connecting fibres patterns

	return os;
}

void Bundle::updatePrimaryInputFibres() {
	std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > & channels =
			inputChannelsMap.getMutableCollection();

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
			boost::shared_ptr<Fibre> temp_fib = this->getPrimaryInputFibreByChannel(it_channels->second->getUUID());

			// apply firing pattern to fibre
			temp_fib->trigger(*pattern);
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
			boost::shared_ptr<state::PatternChannel> patchan = this->getPrimaryOutputChannelByFibre(
					it_outputFibres->first);
			if (patchan != 0) {
				// get pattern from fibre and add it to output channel
				patchan->addPattern(it_outputFibres->second->getInputNodesPattern());
			} else {
				std::cout << "Bundle::updatePrimaryOutputFibres: "
						<< "WARNING: Ignoring primary output fibre since cannot find corresponding channel: "
						<< it_outputFibres->first << std::endl;
			}
			++it_outputFibres;
		}
	}

	// and actually update fibres, not actually required at the moment since we're using the input nodes as the end result
	outputFibres.update();
}

boost::shared_ptr<Fibre> Bundle::getPrimaryInputFibreByChannel(const boost::uuids::uuid id) {
	return this->getPrimaryFibreByChannel(id, inputFibres);
}
boost::shared_ptr<Fibre> Bundle::getPrimaryOutputFibreByChannel(const boost::uuids::uuid id) {
	return this->getPrimaryFibreByChannel(id, outputFibres);
}
boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryInputChannelByFibre(const boost::uuids::uuid id) {
	return this->getPrimaryChannelByFibre(id, inputChannelsMap);
}
boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryOutputChannelByFibre(const boost::uuids::uuid id) {
	return this->getPrimaryChannelByFibre(id, outputChannelsMap);
}
boost::shared_ptr<Fibre> Bundle::getPrimaryFibreByChannel(const boost::uuids::uuid id, FibreMap & map) {
	boost::uuids::uuid fibre_id;
	boost::shared_ptr<Fibre> found_fibre;
	// forall in fibrePatternChannelMap
	{
		bool found = false;
		std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibrePatternChannelMap =
				fibrePatternChannelMap.begin();
		const std::map<boost::uuids::uuid, boost::uuids::uuid>::const_iterator it_fibrePatternChannelMap_end =
				fibrePatternChannelMap.end();
		while (it_fibrePatternChannelMap != it_fibrePatternChannelMap_end && found != true) {
			if (it_fibrePatternChannelMap->second == id) {
				fibre_id = it_fibrePatternChannelMap->first;
				found = true;
			}
			++it_fibrePatternChannelMap;
		}
	}

	if (fibre_id != boost::uuids::nil_uuid()) {
		std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::iterator it_fibre_found =
				map.getMutableCollection().find(fibre_id);
		if (it_fibre_found != map.getMutableCollection().end()) {
			found_fibre = it_fibre_found->second;
		} else {
			std::cout << "Bundle::getPrimaryFibreByChannel: " << "WARNING: Cannot find primary fibre in map: "
					<< fibre_id << std::endl;
		}
	} else {
		std::cout << "Bundle::getPrimaryFibreByChannel: " << "WARNING: Cannot find mapped pattern channel: " << id
				<< std::endl;
	}
	return found_fibre;
}
boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryChannelByFibre(const boost::uuids::uuid id,
		state::PatternChannelMap & map) {
	boost::uuids::uuid pattern_channel_id;
	boost::shared_ptr<state::PatternChannel> found_channel;

	std::map<boost::uuids::uuid, boost::uuids::uuid>::iterator it_found = fibrePatternChannelMap.find(id);
	if (it_found != fibrePatternChannelMap.end()) {
		pattern_channel_id = it_found->second;

		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::iterator it_patchan_found =
				map.getMutableCollection().find(pattern_channel_id);
		if (it_patchan_found != map.getCollection().end()) {
			found_channel = it_patchan_found->second;
		} else {
			std::cout << "Bundle::getPrimaryChannelByFibre: " << "WARNING: Cannot find pattern channel in map: "
					<< pattern_channel_id << std::endl;
		}
	} else {
		std::cout << "Bundle::getPrimaryChannelByFibre: " << "WARNING: Cannot find mapped fibre: " << id << std::endl;
	}

	return found_channel;
}

}//NAMESPACE

}
//NAMESPACE
