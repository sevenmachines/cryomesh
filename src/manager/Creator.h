/*
 * Creator.h
 *
 *  Created on: 29 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CREATOR_H_
#define CREATOR_H_

#include <string>
namespace cryomesh {

namespace manager {

/**
 * Class to take in a config file of ConfigTranslator form and parse the commands to
 * create a full cryomesh object.
 * 	- needs to hold a fakeid to realid mapping to facillitate using fake ids in config files
 * 	before the objects ids are actually known
 * 	- holds a unique instance of any create cryomeshes
 * 	- holds other data from the config file or defaults such as database name, filename,
 * 	data filename, etc
 */
class Creator {
public:
	Creator(const std::string & config_filename);
	Creator(const std::string & config_filename, const std::string & inputdata_filename,
			const std::string & database_filename = "cryomesh.db");

	virtual ~Creator();

	static const std::string DEFAULT_DATABASE_FILENAME;

protected:
	void initialise();
	void createFromConfig();

private:
	std::string configFilename;
	std::string databaseFilename;
	std::string inputDataFilename;
};

}

}

#endif /* CREATOR_H_ */
