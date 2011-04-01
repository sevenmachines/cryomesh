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
	// update all fibres
	fibres.update();
	// update all clusters
	clusters.update();

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
		}else{
			std::cout<<"Bundle::connectCluster (uuid, FibreType, int ) : "<<"WARNING: Ignoring improper FibreType connection call"<<std::endl;
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

}//NAMESPACE

}//NAMESPACE
