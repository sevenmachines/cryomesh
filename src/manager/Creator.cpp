/*
 * Creator.cpp
 *
 *  Created on: 29 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "Creator.h"
#include <algorithm>
#include "common/Containers.h"

namespace cryomesh {

namespace manager {

const std::string Creator::DEFAULT_DATABASE_FILENAME = "cryomesh_default.db";
std::map<std::string, std::list<std::string> > Creator::acceptedCommandList = Creator::getAcceptedCommandList();

std::map<std::string, std::list<std::string> > Creator::getAcceptedCommandList() {
	std::map<std::string, std::list<std::string> > commap;
	//create-cluster id=1 size=100 connectivity=10
	commap["create-cluster"] = std::list<std::string>( { "id", "size", "connectivity" });

	//connect-clusters inputid=1 ouputid=2 width=10
	commap["connect-clusters"] = std::list<std::string>( { "inputid", "ouputid", "width" });

	//create-primary-input  id=2 ouputid=1 width=10
	commap["create-primary-input"] = std::list<std::string>( { "id", "ouputid", "width" });

	//create-primary-output  id=4 inputid=1 width=10
	commap["create-primary-output"] = std::list<std::string>( { "id", "ouputid", "width" });

	//loaddata datafile
	commap["loaddata-cluster"] = std::list<std::string>( { "datafile" });

	//set-primary-in id=2
	commap["set-primary-in"] = std::list<std::string>( { "id" });

	//set-primary-out id=4
	commap["set-primary-out"] = std::list<std::string>( { "id" });

	return commap;
}

Creator::Creator(const std::string & config_filename) :
	configFilename(config_filename), inputDataFilename(""), databaseFilename(Creator::DEFAULT_DATABASE_FILENAME) {

}

Creator::Creator(const std::string & config_filename, const std::string & inputdata_filename,
		const std::string & database_filename) :
	configFilename(config_filename), inputDataFilename(""), databaseFilename(database_filename) {

}

Creator::~Creator() {
}

void Creator::initialise() {
	bundle = boost::shared_ptr<structures::Bundle>(new structures::Bundle);
}

bool Creator::createFromConfig() {
	bool success = true;
	std::ifstream ifs(configFilename.c_str());
	if (ifs.is_open() != true) {
		std::cout << "Creator::createFromConfig: " << "ERROR: File " << "'" << configFilename << "'"
				<< "could not be opened... " << std::endl;
		success = false;
	} else {
		config::ConfigTranslator conf_trans(configFilename);
		bool config_coherent = this->analyseConfig(conf_trans);

		if (config_coherent == true) {

			// iterate through list, running functions
			const std::list<config::ConfigEntry> & conf_entries = conf_trans.getEntries();
			// forall in conf_entries
			{
				std::list<config::ConfigEntry>::const_iterator it_conf_entries = conf_entries.begin();
				const std::list<config::ConfigEntry>::const_iterator it_conf_entries_end = conf_entries.end();
				while (it_conf_entries != it_conf_entries_end) {
					std::string command = it_conf_entries->getCommand();

					if (command == "create-cluster") {
						//create-cluster id=1 size=100 connectivity=10
						int id = it_conf_entries->getIntegerFormattedOptionValue("id");
						int size = it_conf_entries->getIntegerFormattedOptionValue("size");
						int connectivity = it_conf_entries->getIntegerFormattedOptionValue("connectivity");
						this->createCluster(id, size, connectivity);
					} else if (command == "connect-cluster") {
						//connect-clusters inputid=1 ouputid=2 width=10
						int inputid = it_conf_entries->getIntegerFormattedOptionValue("inputid");
						int ouputid = it_conf_entries->getIntegerFormattedOptionValue("ouputid");
						int width = it_conf_entries->getIntegerFormattedOptionValue("width");
						this->connectCluster(inputid, ouputid, width);
					} else if (command == "create-primary-input") {
						//create-primary-input  id=2 ouputid=1 width=10
						int id = it_conf_entries->getIntegerFormattedOptionValue("id");
						int ouputid = it_conf_entries->getIntegerFormattedOptionValue("ouputid");
						int width = it_conf_entries->getIntegerFormattedOptionValue("width");
						this->createPrimaryInputFibre(id, ouputid, width);
					} else if (command == "create-primary-output") {
						//create-primary-output  id=4 inputid=1 width=10
						int id = it_conf_entries->getIntegerFormattedOptionValue("id");
						int inputid = it_conf_entries->getIntegerFormattedOptionValue("inputid");
						int width = it_conf_entries->getIntegerFormattedOptionValue("width");
						this->createPrimaryOutputFibre(id, inputid, width);
					} else if (command == "set-primary-in") {
						//set-primary-in id=2
						int id = it_conf_entries->getIntegerFormattedOptionValue("id");
						this->setPrimaryInputFibre(id);
					} else if (command == "set-primary-out") {
						//set-primary-out id=4
						int id = it_conf_entries->getIntegerFormattedOptionValue("id");
						this->setPrimaryOutputFibre(id);
					} else if (command == "loaddata") {
						//loaddata datafile
						std::string datafile = it_conf_entries->getOptionValue("datafile");
						this->loadData(datafile);
					} else {
						std::cout << "Creator::createFromConfig: " << "ERROR: Unknown command " << "'" << command
								<< "'" << std::endl;
					}

					++it_conf_entries;
				}
			}
		} else {
			success = false;
		}
	}
	return success;
}

bool Creator::analyseConfig(const config::ConfigTranslator & conf_trans) const {
	bool config_coherent = true;
	// verify commands and options
	const std::list<config::ConfigEntry> & conf_entries = conf_trans.getEntries();
	// forall in conf_entries
	{
		std::list<config::ConfigEntry>::const_iterator it_conf_entries = conf_entries.begin();
		const std::list<config::ConfigEntry>::const_iterator it_conf_entries_end = conf_entries.end();
		while (it_conf_entries != it_conf_entries_end) {
			config_coherent = config_coherent && this->checkConfigEntry(*it_conf_entries);
			++it_conf_entries;
		}
	}

	config_coherent = config_coherent && this->checkConfigStructure(conf_entries);

	return config_coherent;
}

bool Creator::checkConfigEntry(const config::ConfigEntry & conf_entry) const {
	bool success = true;
	// check command is a member
	std::map<std::string, std::list<std::string> >::const_iterator it_found = Creator::acceptedCommandList.find(
			conf_entry.getCommand());
	if (it_found == Creator::acceptedCommandList.end()) {
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

bool Creator::checkConfigStructure(const std::list<config::ConfigEntry> & conf_entries) const {
	return false;
}
// Creation function mappings
void Creator::createCluster(int id, int size, int connectivity) {
	std::cout << "Creator::createCluster: " << "" << std::endl;
	clusterIDMap[id] =  bundle->createCluster(size, connectivity)->getUUID();
}
void Creator::connectCluster(int input_cluster_id, int output_cluster_id, int width) {
	std::cout << "Creator::connectCluster: " << "" << std::endl;
	boost::uuids::uuid input_cluster_real_uuid = getClusterRealID(input_cluster_id);
	boost::uuids::uuid output_cluster_real_uuid = getClusterRealID(output_cluster_id);
	if (input_cluster_real_uuid != boost::uuids::nil_uuid() && output_cluster_real_uuid != boost::uuids::nil_uuid()) {
		bundle->connectCluster(input_cluster_real_uuid, output_cluster_real_uuid, width);
	}
}
void Creator::createPrimaryInputFibre(int id, int output_cluster_id, int width) {
	std::cout << "Creator::createPrimaryInputFibre: " << "" << std::endl;
	boost::uuids::uuid real_uuid = getClusterRealID(output_cluster_id);
	if (real_uuid != boost::uuids::nil_uuid()) {
		bundle->connectPrimaryInputCluster(real_uuid, width);
		fibreIDMap[id] = real_uuid;
	}
}
void Creator::createPrimaryOutputFibre(int id, int input_cluster_id, int width) {
	boost::uuids::uuid real_uuid = getClusterRealID(input_cluster_id);
	if (real_uuid != boost::uuids::nil_uuid()) {
		bundle->connectPrimaryOutputCluster(real_uuid, width);
		fibreIDMap[id] = real_uuid;
	}
}
void Creator::loadData(std::string datafile) {
	std::cout << "Creator::loadData: " << "" << std::endl;
}
void Creator::setPrimaryInputFibre(int id) {
	std::cout << "Creator::setPrimaryInputFibre: " << "" << std::endl;
}
void Creator::setPrimaryOutputFibre(int id) {
	std::cout << "Creator::setPrimaryOutputFibre: " << "" << std::endl;
}
boost::uuids::uuid Creator::getRealID(const int id, const std::map<int, boost::uuids::uuid> & idmap) const {
	boost::uuids::uuid found_id;
	const std::map<int, boost::uuids::uuid>::const_iterator it_found = idmap.find(id);
	if (it_found != idmap.end()) {
		found_id = it_found->second;
	} else {
		std::cout << "Creator::getRealID: " << "ERROR: Real id " << "'" << id << "'" << " not found." << std::endl;

	}
	return found_id;
}

boost::uuids::uuid Creator::getClusterRealID(const int id) const {
	return getRealID(id, clusterIDMap);
}

boost::uuids::uuid Creator::getFibreRealID(const int id) const {
	return getRealID(id, fibreIDMap);
}
}//NAMESPACE

}
//NAMESPACE
