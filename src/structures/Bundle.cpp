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

	// TODO do evolution things

	// TODO do things with storing output results from channels

	// TODO Debugging things
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
	seqchans.readSequences(ifstr, inputChannelsMap, outputChannelsMap);
	// get list of input channels
	//	inputChannelsMap = seqchans.getInputChannelsMap();

	// ditto for output channels
	//outputChannelsMap = seqchans.getOutputChannelsMap();
	std::cout << "Bundle::loadChannels: " << "" << std::endl;
	this->printChannels(std::cout);
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

FibreMap & Bundle::getMutableInputFibres() {
	return inputFibres;
}

const FibreMap & Bundle::getOutputFibres() const {
	return outputFibres;
}

FibreMap & Bundle::getMutableOutputFibres() {
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

const boost::shared_ptr<utilities::Statistician> Bundle::getStatistician() const {
	return statistician;
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
			if (temp_fib != 0) {
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
			boost::shared_ptr<state::PatternChannel> patchan = this->getPrimaryOutputChannelByFibre(
					it_outputFibres->first);
			if (patchan != 0) {
				// get pattern from fibre and add it to output channel
				boost::shared_ptr<state::Pattern> temp_pattern = it_outputFibres->second->getInputNodesPattern();
				std::cout << "Bundle::updatePrimaryOutputFibres: " << temp_pattern->getString() << std::endl;
				patchan->addPattern(temp_pattern);
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
boost::shared_ptr<Fibre> Bundle::getPrimaryInputFibreByChannel(const boost::uuids::uuid pattern_channel_uuid) {
	//std::cout << "Bundle::getPrimaryInputFibreByChannel: " << "" << std::endl;
	//	this->printSearch<state::PatternChannel> (std::cout, pattern_channel_uuid, inputChannelsMap.getCollection());
	//this->printSearch<Fibre> (std::cout, pattern_channel_uuid, inputFibres.getCollection());
	return this->getPrimaryFibreByChannel(pattern_channel_uuid, inputFibres);
}
boost::shared_ptr<Fibre> Bundle::getPrimaryOutputFibreByChannel(const boost::uuids::uuid pattern_channel_uuid) {
	//std::cout << "Bundle::getPrimaryOutputFibreByChannel: " << "" << std::endl;
	//this->printSearch<Fibre> (std::cout, pattern_channel_uuid, outputFibres.getCollection());

	return this->getPrimaryFibreByChannel(pattern_channel_uuid, outputFibres);
}
boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryInputChannelByFibre(const boost::uuids::uuid fibre_uuid) {
	//	std::cout << "Bundle::getPrimaryInputChannelByFibre: " << "" << std::endl;
	//	this->printSearch<state::PatternChannel> (std::cout, fibre_uuid, inputChannelsMap.getCollection());
	return this->getPrimaryChannelByFibre(fibre_uuid, inputChannelsMap);
}
boost::shared_ptr<state::PatternChannel> Bundle::getPrimaryOutputChannelByFibre(const boost::uuids::uuid fibre_uuid) {
	//	std::cout << "Bundle::getPrimaryOutputChannelByFibre: " << "" << std::endl;
	//	this->printSearch<state::PatternChannel> (std::cout, fibre_uuid, outputChannelsMap.getCollection());
	return this->getPrimaryChannelByFibre(fibre_uuid, outputChannelsMap);
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

template<class T>
std::ostream & Bundle::printSearch(std::ostream & os, const boost::uuids::uuid & uuid,
		const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & map) {
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
	}
	os << std::endl << "***********************************************" << std::endl << std::endl;
	return os;
}

std::ostream& Bundle::printChannels(std::ostream & os) const {
	std::cout << " Bundle::printChannels: " << "" << std::endl;
	const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > in_channels =
			inputChannelsMap.getCollection();
	const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > out_channels =
			outputChannelsMap.getCollection();

	// forall in in_channels
	{
		os << "InChannels -> {";
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
		os << "OutChannels -> {";
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
	return os;
}

std::ostream& operator<<(std::ostream & os, const Bundle & obj) {
	std::cout << "Bundle::operator <<: " << "" << std::endl;
	return obj.print(os, common::Loggable::MAX);
}

}//NAMESPACE

}//NAMESPACE
