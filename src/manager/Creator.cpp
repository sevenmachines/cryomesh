/*
 * Creator.cpp
 *
 *  Created on: 29 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "Creator.h"

namespace cryomesh {

namespace manager {

const std::string Creator::DEFAULT_DATABASE_FILENAME = "cryomesh_default.db";

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

}

void Creator::createFromConfig() {

}

}//NAMESPACE

}//NAMESPACE
