/*
 * Creator.h
 *
 *  Created on: 29 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CREATOR_H_
#define CREATOR_H_

#include "config/ConfigTranslator.h"

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <structures/Bundle.h>

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
			const std::string & database_filename = DEFAULT_DATABASE_FILENAME);

	virtual ~Creator();

	boost::shared_ptr< structures::Bundle > getBundle();

	bool analyseConfig(const config::ConfigTranslator & conf_trans) const;
	bool checkConfigEntry(const config::ConfigEntry & conf_entry) const;
	bool checkConfigStructure(const std::list<config::ConfigEntry> & conf_entries) const;

	// Creation function mappings
	void createCluster(int id, int size, int connectivity);
	void connectCluster(int input_cluster_id, int ouput_cluster_id, int width);
	void loadData(std::string datafile);
	void connectPrimaryInputFibre(int id, int outputid);
	void connectPrimaryOutputFibre(int id, int inputid);

	static const std::string DEFAULT_DATABASE_FILENAME;

	/**
	 * Map of accepted commands to a list of options
	 */
	static std::map<std::string, std::list<std::string> > acceptedCommandList;
	static std::map<std::string, std::list<std::string> > getAcceptedCommandList();

protected:
	void initialise();
	bool createFromConfig();

private:
	std::string configFilename;
	std::string inputDataFilename;
	std::string databaseFilename;
	boost::shared_ptr< structures::Bundle > bundle;

	/**
	 * Map of cluster real uuid's to config files fake id's
	 */
	 std::map<int, boost::uuids::uuid>  clusterIDMap;

	/**
	 * Map of fibre real uuid's to config files fake id's
	 */
	 std::map<int, boost::uuids::uuid>  fibreIDMap;

		/**
		 * Map of pattern channel real uuid's to config files fake id's
		 */
		 std::map<int, boost::uuids::uuid> patternChannelIDMap;

	/**
	 * Retreive a uuid from a fake int id
	 *
	 */
	boost::uuids::uuid getRealID(const int id, const std::map<int, boost::uuids::uuid> & idmap) const;
	boost::uuids::uuid getClusterRealID(const int id) const;
	boost::uuids::uuid getFibreRealID(const int id) const;
	boost::uuids::uuid getPatternChannelRealID(const int id) const;
};

}

}

#endif /* CREATOR_H_ */
