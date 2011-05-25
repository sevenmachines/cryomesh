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
	/**
	 * Constructor to create a bundle from a config file name with option
	 * to specify the database file name that will be used
	 *
	 * @param std::string
	 * 	The name with full path of the config file
	 * @param std::string
	 * 	The name with full path of the database file
	 */
	Creator(const std::string & config_filename, const std::string & database_filename = DEFAULT_DATABASE_FILENAME);
	Creator( std::istream & config_stream, const std::string & database_filename = DEFAULT_DATABASE_FILENAME);

	/**
	 * Deffault destructor
	 */
	virtual ~Creator();

	/**
	 * Get the create bundle
	 *
	 * @return boost::shared_ptr<structures::Bundle>
	 * 	The created bundle
	 */
	const boost::shared_ptr<structures::Bundle> getBundle() const ;

	/**
	 * Get the mutable create bundle
	 *
	 * @return boost::shared_ptr<structures::Bundle>
	 * 	The created bundle
	 */
	boost::shared_ptr<structures::Bundle> getMutableBundle();

	/**
	 * get the clusterIDMap
	 *
	 * @return const std::map<int, boost::uuids::uuid>
	 * 	the clusterIDMap
	 */
	const std::map<int, boost::uuids::uuid> & getClusterIDMap() const;

	/**
	 * get the fibreIDMap
	 *
	 * @return const std::map<int, boost::uuids::uuid>
	 * 	the fibreIDMap
	 */
	const std::map<int, boost::uuids::uuid> & getFibreIDMap() const;

	/**
	 * get the patternChannelIDMap
	 *
	 * @return const std::map<int, boost::uuids::uuid>
	 * 	the patternChannelIDMap
	 */
	const std::map<int, boost::uuids::uuid> & getPatternChannelIDMap() const;

	/**
	 * Analyse the config translator for coherence
	 *
	 * @param ConfigTranslator
	 * 	The config translator to analyse
	 *
	 * @return bool
	 * 	True if  the config translator passed all the tests for coherence, false otherwise.
	 */
	static bool analyseConfig(const config::ConfigTranslator & conf_trans) ;

	/**
	 * Analyse the config entry for coherence
	 *
	 * @param ConfigEntry
	 * 	The config entry to analyse
	 *
	 * @return bool
	 * 	True if the config entry passed all the tests for coherence, false otherwise.
	 */
	static bool checkConfigEntry(const config::ConfigEntry & conf_entry) ;

	/**
	 * Analyse the config for structural coherence
	 *
	 * @param std::list<config::ConfigEntry>
	 * 	The list of config entries to analyse for structure
	 *
	 * @return bool
	 * 	True if the config entries passed all the tests for structural coherence, false otherwise.
	 */
	static bool checkConfigStructure(const std::list<config::ConfigEntry> & conf_entries) ;

	// Creation function mappings

	/**
	 * Translator from config command to actual command.
	 * Create a cluster using a fake id to map to a real one
	 *
	 * @param int
	 * 	The fake id of the cluster
	 * @param int
	 * 	The size of the cluster
	 * @param int
	 * 	The connetivity of the cluster
	 */
	void createCluster(int id, int size, int connectivity);

	/**
	 * Translator from config command to actual command.
	 * Connect two clusters using there fake ids
	 *
	 * @param int
	 * 	The fake id of the input cluster
	 * @param int
	 * 	The fake id of the output cluster
	 * @param int
	 * 	The width of the new fibre connection
	 */
	void connectCluster(int input_cluster_id, int ouput_cluster_id, int width);

	/**
	 * Translator from config command to actual command.
	 * Load the pattern data in from a file
	 *
	 * @param std::string
	 * 	The full file path name of the pattern data file
	 */
	void loadData(std::string datafile);

	/**
	 * Translator from config command to actual command.
	 * Create a fibre to connect a primary input pattern channel to a cluster output
	 *
	 * @param int
	 * 	The fake id of the pattern channel
	 * @param
	 * 	The fake id of the output cluster
	 */
	void connectPrimaryInputChannel(int channel_id, int outputid);

	/**
	 * Translator from config command to actual command.
	 * Create a fibre to connect a primary output pattern channel to a cluster output
	 *
	 * @param int
	 * 	The fake id of the pattern channel
	 * @param
	 * 	The fake id of the input cluster
	 */
	void connectPrimaryOutputChannel(int channel_id, int inputid);

	/**
	 * auto connect all the primary input channels to the list of clusters
	 *
	 * @param std::vector<int>
	 * 	The fake ids of the clusters to connect
	 */
	void autoConnectPrimaryInputs(const std::vector<int> & cluster_ids);

	/**
	 * auto connect all the primary output channels to the list of clusters
	 *
	 * @param std::vector<int>
	 * 	The fake ids of the clusters to connect
	 */
	void autoConnectPrimaryOutputs(const std::vector<int> & cluster_ids);

	/**
	 * The default database filename to use
	 *
	 * @var std::string
	 */
	static const std::string DEFAULT_DATABASE_FILENAME;

	/**
	 * Map of accepted commands to a list of options
	 *
	 * @var std::map<std::string, std::list<std::string> >
	 */
	static std::map<std::string, std::list<std::string> > acceptedCommandList;

	/**
	 * Generate and return the accepted command list
	 *
	 * @return std::map<std::string, std::list<std::string> >
	 * 	The accepted commands mapping
	 */
	static std::map<std::string, std::list<std::string> > getAcceptedCommandList();

protected:

	/**
	 * Helper to initialise the creator
	 */
	void initialise();

	/**
	 * Run through the config file generating all commands
	 *
	 * @return bool
	 * 	True if running the config file was successful, false otherwise
	 */
	bool createFromConfigFile(const std::string config_filename);
	bool createFromConfigStream(std::istream & is);

private:
	/**
	 * The full path name of the config file
	 *
	 * @var std::string
	 */
	//std::string configFilename;

	/**
	 * The full path name of the pattern data set file
	 *
	 * @var std::string
	 */

	//std::string inputDataFilename;

	/**
	 * The full path name of the database file
	 *
	 * @var std::string
	 */
	std::string databaseFilename;

	/**
	 * The bundle that is created
	 *
	 * @var boost::shared_ptr<structures::Bundle>
	 */
	boost::shared_ptr<structures::Bundle> bundle;

	/**
	 * Map of cluster real uuid's to config files fake id's
	 *
	 * @var std::map<int, boost::uuids::uuid>
	 */
	std::map<int, boost::uuids::uuid> clusterIDMap;

	/**
	 * Map of fibre real uuid's to config files fake id's
	 *
	 * @var std::map<int, boost::uuids::uuid>
	 */
	std::map<int, boost::uuids::uuid> fibreIDMap;

	/**
	 * Map of pattern channel real uuid's to config files fake id's
	 *
	 * @var std::map<int, boost::uuids::uuid>
	 */
	std::map<int, boost::uuids::uuid> patternChannelIDMap;

	/**
	 * Retreive a uuid from a fake int id inside a map
	 *
	 *@param int
	 *		The fake id to translate
	 *@param std::map<int, boost::uuids::uuid>
	 *		The map to use for translation
	 *
	 *@return boost::uuids::uuid
	 *		The corresponding real uuid to the fake one, null if it doesnt exist
	 */
	boost::uuids::uuid getRealID(const int id, const std::map<int, boost::uuids::uuid> & idmap) const;

	/**
	 * Helper to retrieve a uuid from a fake int id for clusters
	 *
	 *@param int
	 *		The fake id to translate
	 *
	 *@return boost::uuids::uuid
	 *		The corresponding real uuid to the fake one, null if it doesnt exist
	 */
	boost::uuids::uuid getClusterRealID(const int id) const;

	/**
	 * Helper to retrieve a uuid from a fake int id for fibres
	 *
	 *@param int
	 *		The fake id to translate
	 *
	 *@return boost::uuids::uuid
	 *		The corresponding real uuid to the fake one, null if it doesnt exist
	 */
	boost::uuids::uuid getFibreRealID(const int id) const;

	/**
	 * Helper to retrieve a uuid from a fake int id for pattern channels
	 *
	 *@param int
	 *		The fake id to translate
	 *
	 *@return boost::uuids::uuid
	 *		The corresponding real uuid to the fake one, null if it doesnt exist
	 */
	boost::uuids::uuid getPatternChannelRealID(const int id) const;
};

}

}

#endif /* CREATOR_H_ */
