/*
 * Creator.cpp
 *
 *  Created on: 29 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

//#define CREATOR_DEBUG

#include "Creator.h"
#include <algorithm>
#include "common/Containers.h"
#include <boost/uuid/uuid_io.hpp>

namespace cryomesh {

namespace manager {

const std::string Creator::DEFAULT_DATABASE_FILENAME = "cryomesh_default.db";
std::map<std::string, std::list<std::string> > Creator::acceptedCommandList = Creator::getAcceptedCommandList();

std::map<std::string, std::list<std::string> > Creator::getAcceptedCommandList() {
	std::map < std::string, std::list<std::string> > commap;
	//create-cluster id=1 size=100 connectivity=10
	commap["create-cluster"] = std::list<std::string>( { "id", "size", "connectivity" });

	//connect-clusters inputid=1 outputid=2 width=10
	commap["connect-clusters"] = std::list<std::string>( { "inputid", "outputid", "width" });

	//loaddata datafile
	commap["loaddata"] = std::list<std::string>( { "file" });

	// connect-primary-input id=1 outputid=1
	commap["connect-primary-input"] = std::list<std::string>( { "id", "outputid" });

	// connect-primary-input id=2 outputid=2
	commap["connect-primary-output"] = std::list<std::string>( { "id", "inputid" });

	// autoconnect-input ids="1 2 3"
	commap["autoconnect-inputs"] = std::list<std::string>( { "ids" });

	// autoconnect-output ids="1 2 3"
	commap["autoconnect-outputs"] = std::list<std::string>( { "ids" });
	return commap;
}

Creator::Creator(const std::string & config_filename, const std::string & database_filename) :
		databaseFilename(database_filename), bundle(), clusterIDMap(), fibreIDMap(), patternChannelIDMap() {
	this->initialise();

	// create from config
	{
		bool success = this->createFromConfigFile(config_filename);
		if (success == false) {
			std::cout << "Creator::Creator: " << "ERROR: Creating bundle from config file " << "'" << config_filename
					<< "'" << std::endl;
		}
	}
}

Creator::Creator(std::istream & config_stream, const std::string & database_filename) :
		databaseFilename(database_filename), bundle(), clusterIDMap(), fibreIDMap(), patternChannelIDMap() {
	this->initialise();

	bool success = this->createFromConfigStream(config_stream);
	if (success == false) {
		std::cout << "Creator::Creator: " << "ERROR: Creating bundle from config stream " << std::endl;
	}
}

Creator::~Creator() {
}

const boost::shared_ptr<structures::Bundle> Creator::getBundle() const {
	return bundle;
}

boost::shared_ptr<structures::Bundle> Creator::getMutableBundle() {
	return bundle;
}

const std::map<int, boost::uuids::uuid> & Creator::getClusterIDMap() const {
	return clusterIDMap;
}

const std::map<int, boost::uuids::uuid> & Creator::getFibreIDMap() const {
	return fibreIDMap;
}

const std::map<int, boost::uuids::uuid> & Creator::getPatternChannelIDMap() const {
	return patternChannelIDMap;
}

void Creator::initialise() {
	bundle = boost::shared_ptr<structures::Bundle>(new structures::Bundle);
}

bool Creator::runCommand(const config::ConfigEntry & conf_entry) {
	std::string command = conf_entry.getCommand();
	bool success = true;
	if (command == "create-cluster") {
		//create-cluster id=1 size=100 connectivity=10
		int id = conf_entry.getIntegerFormattedOptionValue("id");
		int size = conf_entry.getIntegerFormattedOptionValue("size");
		int connectivity = conf_entry.getIntegerFormattedOptionValue("connectivity");
		this->createCluster(id, size, connectivity);
	} else if (command == "connect-clusters") {
		//connect-clusters inputid=1 outputid=2 width=10
		int inputid = conf_entry.getIntegerFormattedOptionValue("inputid");
		int outputid = conf_entry.getIntegerFormattedOptionValue("outputid");
		int width = conf_entry.getIntegerFormattedOptionValue("width");
		this->connectCluster(inputid, outputid, width);
		//		} else if (command == "create-primary-input") {
		//			//create-primary-input  id=2 outputid=1 width=10
		//			int id = conf_entry.getIntegerFormattedOptionValue("id");
		//				int outputid = conf_entry.getIntegerFormattedOptionValue("outputid");
		//				int width = conf_entry.getIntegerFormattedOptionValue("width");
		//				this->createPrimaryInputFibre(id, outputid, width);
		//			} else if (command == "create-primary-output") {
		//				//create-primary-output  id=4 inputid=1 width=10
		//				int id = conf_entry.getIntegerFormattedOptionValue("id");
		//				int inputid = conf_entry.getIntegerFormattedOptionValue("inputid");
		//				int width = conf_entry.getIntegerFormattedOptionValue("width");
		//				this->createPrimaryOutputFibre(id, inputid, width);
	} else if (command == "connect-primary-input") {
		// connect-primary-input id=1 outputid=1
		int id = conf_entry.getIntegerFormattedOptionValue("id");
		int outputid = conf_entry.getIntegerFormattedOptionValue("outputid");
		this->connectPrimaryInputChannel(id, outputid);
	} else if (command == "autoconnect-inputs") {
		//autoconnect-inputs ids="1 2 3"
		std::vector<int> ids = config::ConfigEntry::toIntegerMultipleValues(
				config::ConfigEntry::tokenizeMultipleValueString(conf_entry.getOptionValue("ids")));
		this->autoConnectPrimaryInputs(ids);
	} else if (command == "autoconnect-outputs") {
		//autoconnect-outputs ids="1 2 3"
		std::vector<int> ids = config::ConfigEntry::toIntegerMultipleValues(
				config::ConfigEntry::tokenizeMultipleValueString(conf_entry.getOptionValue("ids")));
		this->autoConnectPrimaryOutputs(ids);
	} else if (command == "connect-primary-output") {
		// connect-primary-output id=2 outputid=2
		int id = conf_entry.getIntegerFormattedOptionValue("id");
		int inputid = conf_entry.getIntegerFormattedOptionValue("inputid");
		this->connectPrimaryOutputChannel(id, inputid);
	} else if (command == "loaddata") {
		//loaddata datafile
		std::string datafile = conf_entry.getOptionValue("file");
		this->loadData(datafile);
	} else {
		std::cout << "Creator::createFromConfig: " << "ERROR: Unknown command " << "'" << command << "'" << std::endl;
		success = false;
	}
	return success;
}

bool Creator::createFromConfigStream(std::istream & is) {
	bool success = true;
	config::ConfigTranslator conf_trans(is);
	bool config_coherent = this->analyseConfig(conf_trans);

	if (config_coherent == true) {

		// iterate through list, running functions
		const std::list<config::ConfigEntry> & conf_entries = conf_trans.getEntries();
		// forall in conf_entries
		{
			std::list<config::ConfigEntry>::const_iterator it_conf_entries = conf_entries.begin();
			const std::list<config::ConfigEntry>::const_iterator it_conf_entries_end = conf_entries.end();
			while (it_conf_entries != it_conf_entries_end) {
				success = success && this->runCommand(*it_conf_entries);
				++it_conf_entries;
			}
		}
	} else {
		std::cout << "Creator::createFromConfig: " << "WARNING: Incoherent config" << std::endl;
		success = false;
	}
	return success;
}

bool Creator::createFromConfigFile(const std::string config_filename) {
	bool success = true;
	std::ifstream ifs(config_filename.c_str());
	if (ifs.is_open() != true) {
		std::cout << "Creator::createFromConfig: " << "ERROR: File " << "'" << config_filename << "'"
				<< "could not be opened... " << std::endl;
		success = false;
		return false;
	} else {
		success = this->createFromConfigStream(ifs);
		return success;
	}
}

bool Creator::analyseConfig(const config::ConfigTranslator & conf_trans) {
	bool config_coherent = true;
	// verify commands and options
	const std::list<config::ConfigEntry> & conf_entries = conf_trans.getEntries();
	// forall in conf_entries
	{
		std::list<config::ConfigEntry>::const_iterator it_conf_entries = conf_entries.begin();
		const std::list<config::ConfigEntry>::const_iterator it_conf_entries_end = conf_entries.end();
		while (it_conf_entries != it_conf_entries_end) {
			bool good_entry = Creator::checkConfigEntry(*it_conf_entries);
			config_coherent = config_coherent && good_entry;
			++it_conf_entries;
		}
	}

	config_coherent = config_coherent && Creator::checkConfigStructure(conf_entries);

	return config_coherent;
}

bool Creator::checkConfigEntry(const config::ConfigEntry & conf_entry) {
	bool success = true;
	// check command is a member
	std::map<std::string, std::list<std::string> >::const_iterator it_found = Creator::acceptedCommandList.find(
			conf_entry.getCommand());
	if (it_found == Creator::acceptedCommandList.end()) {
		std::cout << "Creator::checkConfigEntry: " << "ERROR: Command not found " << "'" << conf_entry.getCommand()
				<< "'" << std::endl;
		success = false;
	} else {
		const std::list<std::string> & needed_options = it_found->second;
		// forall in needed_options
		{
			std::list<std::string>::const_iterator it_needed_options = needed_options.begin();
			const std::list<std::string>::const_iterator it_needed_options_end = needed_options.end();
			while (it_needed_options != it_needed_options_end) {
				// search for all accepted options in config entry options
				if (conf_entry.getOptions().find(*it_needed_options) == conf_entry.getOptions().end()) {
					std::cout << "Creator::checkConfigEntry: " << "ERROR: Cannot find required option " << "'"
							<< *it_needed_options << "'" << " in ConfigEntry " << conf_entry.getRawEntry() << std::endl;
					success = false;
				}
				++it_needed_options;
			}

		}
	}
	return success;
}

bool Creator::checkConfigStructure(const std::list<config::ConfigEntry> & conf_entries) {
	bool success = true;
	// check we have at least one cluster
	bool create_cluster_found = false;
	// check we have at least one data load
	bool loaddata_found = false;

	// forall in conf_entries
	{
		std::list<config::ConfigEntry>::const_iterator it_conf_entries = conf_entries.begin();
		const std::list<config::ConfigEntry>::const_iterator it_conf_entries_end = conf_entries.end();
		while (it_conf_entries != it_conf_entries_end && ((create_cluster_found != true) || (loaddata_found != true))) {
			//std::cout << "Creator::checkConfigStructure: " << "'" << it_conf_entries->getCommand()<< "'"  << std::endl;
			if (it_conf_entries->getCommand() == "create-cluster") {
				create_cluster_found = true;
			} else if (it_conf_entries->getCommand() == "loaddata") {
				loaddata_found = true;
			}
			++it_conf_entries;
		}

		if (create_cluster_found != true) {
			std::cout << "Creator::checkConfigStructure: "
					<< "WARNING: Config file has no 'create-cluster' command... " << std::endl;
		} else if (loaddata_found != true) {
			std::cout << "Creator::checkConfigStructure: " << "WARNING: Config file has no 'loaddata' command... "
					<< std::endl;
		}
	}

	success = success && create_cluster_found;
	success = success && loaddata_found;
	return success;
}
// Creation function mappings
void Creator::createCluster(int id, int size, int connectivity) {
	//	std::cout << "Creator::createCluster: " << "(" << id << ", " << size << ", " << connectivity << ")" << std::endl;
	clusterIDMap[id] = bundle->createCluster(size, connectivity)->getUUID();
}
void Creator::connectCluster(int input_cluster_id, int output_cluster_id, int width) {
	//std::cout << "Creator::connectCluster: " << "(" << input_cluster_id << ", " << output_cluster_id << ", " << width
	//			<< ")" << std::endl;
	boost::uuids::uuid input_cluster_real_uuid = getClusterRealID(input_cluster_id);
	boost::uuids::uuid output_cluster_real_uuid = getClusterRealID(output_cluster_id);
	if (input_cluster_real_uuid != boost::uuids::nil_uuid() && output_cluster_real_uuid != boost::uuids::nil_uuid()) {
		bundle->connectCluster(input_cluster_real_uuid, output_cluster_real_uuid, width);
	}
}

//DEPRECATED
/*
 void Creator::createPrimaryInputFibre(int id, int output_cluster_id, int width) {
 std::cout << "Creator::createPrimaryInputFibre: " << "(" << id << ", " << output_cluster_id << ", " << width << ")"
 << std::endl;
 boost::uuids::uuid real_uuid = getClusterRealID(output_cluster_id);
 if (real_uuid != boost::uuids::nil_uuid()) {
 bundle->connectPrimaryInputCluster(real_uuid, width);
 fibreIDMap[id] = real_uuid;
 }
 }
 void Creator::createPrimaryOutputFibre(int id, int input_cluster_id, int width) {
 std::cout << "Creator::createPrimaryOutputFibre: " << "(" << id << ", " << input_cluster_id << ", " << width << ")"
 << std::endl;
 boost::uuids::uuid real_uuid = getClusterRealID(input_cluster_id);
 if (real_uuid != boost::uuids::nil_uuid()) {
 bundle->connectPrimaryOutputCluster(real_uuid, width);
 fibreIDMap[id] = real_uuid;
 }
 }*/

void Creator::loadData(std::string datafile) {
	bundle->loadChannels(datafile);
	// map inputs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_chans =
				bundle->getRealInputChannelsMap().begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_chans_end =
				bundle->getRealInputChannelsMap().end();
		while (it_chans != it_chans_end) {
			patternChannelIDMap[it_chans->second->getRefID()] = it_chans->first;
			++it_chans;
		}
	}

	// map outputs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_chans =
				bundle->getRealOutputChannelsMap().begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator it_chans_end =
				bundle->getRealOutputChannelsMap().end();
		while (it_chans != it_chans_end) {
			patternChannelIDMap[it_chans->second->getRefID()] = it_chans->first;
			++it_chans;
		}
	}

}
void Creator::connectPrimaryInputChannel(int channel_id, int outputid) {
	//	std::cout << "Creator::connectPrimaryInputFibre: " << "(" << channel_id << ", " << outputid << ")" << std::endl;
	//	std::cout << "Creator::connectPrimaryOutputFibre: " << "(" << this->getPatternChannelRealID(channel_id) << ", "
	//			<< this->getClusterRealID(outputid) << ")" << std::endl;
	bundle->connectPrimaryInputCluster(this->getPatternChannelRealID(channel_id), this->getClusterRealID(outputid));

}
void Creator::connectPrimaryOutputChannel(int channel_id, int inputid) {
	//std::cout << "Creator::connectPrimaryOutputFibre: " << "(" << channel_id << ", " << inputid << ")" << std::endl;
	//std::cout << "Creator::connectPrimaryOutputFibre: " << "(" << this->getPatternChannelRealID(channel_id) << ", "
	//		<< this->getClusterRealID(inputid) << ")" << std::endl;
	bundle->connectPrimaryOutputCluster(this->getPatternChannelRealID(channel_id), this->getClusterRealID(inputid));
}

void Creator::autoConnectPrimaryInputs(const std::vector<int> & cluster_ids) {
#ifdef CREATOR_DEBUG
	std::cout << "Creator::autoConnectPrimaryInputs: " << "" << std::endl;
#endif
	// get all real ids
	std::vector < boost::uuids::uuid > real_ids;
	// forall in cluster_ids
	{
		std::vector<int>::const_iterator it_cluster_ids = cluster_ids.begin();
		const std::vector<int>::const_iterator it_cluster_ids_end = cluster_ids.end();
		while (it_cluster_ids != it_cluster_ids_end) {
			real_ids.push_back(this->getClusterRealID(*it_cluster_ids));
			++it_cluster_ids;
		}
	}
	bundle->autoConnectPrimaryInputClusters(real_ids);
}

void Creator::autoConnectPrimaryOutputs(const std::vector<int> & cluster_ids) {
#ifdef CREATOR_DEBUG
	std::cout << "Creator::autoConnectPrimaryOutputs: " << "" << std::endl;
#endif
	// get all real ids
	std::vector < boost::uuids::uuid > real_ids;
	// forall in cluster_ids
	{
		std::vector<int>::const_iterator it_cluster_ids = cluster_ids.begin();
		const std::vector<int>::const_iterator it_cluster_ids_end = cluster_ids.end();
		while (it_cluster_ids != it_cluster_ids_end) {
			real_ids.push_back(this->getClusterRealID(*it_cluster_ids));
			++it_cluster_ids;
		}
	}
	bundle->autoConnectPrimaryOutputClusters(real_ids);
}

boost::uuids::uuid Creator::getRealID(const int id, const std::map<int, boost::uuids::uuid> & idmap) const {

	boost::uuids::uuid found_id;
	const std::map<int, boost::uuids::uuid>::const_iterator it_found = idmap.find(id);
	if (it_found != idmap.end()) {
		found_id = it_found->second;
	} else {
		std::cout << "Creator::getRealID: " << "ERROR: Real id " << "'" << id << "'" << " not found." << std::endl;
		// forall in idmap
		{
			std::cout << "idmap: {" << std::endl;
			std::map<int, boost::uuids::uuid>::const_iterator it_idmap = idmap.begin();
			const std::map<int, boost::uuids::uuid>::const_iterator it_idmap_end = idmap.end();
			while (it_idmap != it_idmap_end) {
				std::cout << "\t" << it_idmap->first << " : " << it_idmap->second << std::endl;
				++it_idmap;
			}
			std::cout << "}" << std::endl;
		}

	}
#ifdef CREATOR_DEBUG
	std::cout << "Creator::getRealID: " << id << " -> " << found_id << std::endl;
#endif
	return found_id;
}

boost::uuids::uuid Creator::getClusterRealID(const int id) const {
#ifdef CREATOR_DEBUG
	std::cout << "Creator::getClusterRealID: " << "" << std::endl;
#endif
	return getRealID(id, clusterIDMap);
}

boost::uuids::uuid Creator::getFibreRealID(const int id) const {
#ifdef CREATOR_DEBUG
	std::cout << "Creator::getFibreRealID: " << "" << std::endl;
#endif
	return getRealID(id, fibreIDMap);
}

boost::uuids::uuid Creator::getPatternChannelRealID(const int id) const {
#ifdef CREATOR_DEBUG
	std::cout << "Creator::getPatternChannelRealID: " << "" << std::endl;
#endif
	return getRealID(id, patternChannelIDMap);
}

}//NAMESPACE

}
//NAMESPACE
