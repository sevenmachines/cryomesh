/*
 * Bundle.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef BUNDLE_H_
#define BUNDLE_H_

#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <iostream>
#include "ClusterMap.h"
#include "FibreMap.h"
#include "state/PatternChannelMap.h"
#include "common/Debuggable.h"
#include "utilities/Statistician.h"

namespace cryomesh {

namespace structures {

/**
 * A Bundle is the collection of clusters and fibres, it represents the system as a whole
 */
class Bundle: public common::Debuggable {
public:

	/**
	 * Default contructor
	 */
	Bundle();

	/**
	 * Default destructor
	 */
	virtual ~Bundle();

	/**
	 * Update all bundle components
	 */
	void update();

	/**
	 * Create a cluster with a size and connectivity
	 *
	 * @param int
	 * 	The number of nodes to create
	 * @param int
	 * 	The connectivity of the nodes
	 *
	 * @return boost::shared_ptr<Cluster>
	 * 	The cluster that was created
	 */
	virtual boost::shared_ptr<Cluster> createCluster(int nodeSize, int nodeConnectivity);

	/**
	 * Connect clusters specified by uuids with a fibre of width
	 *
	 * @param boost::uuids::uuid inputClusterUUID
	 * 		UUID of input cluster
	 * @param boost::uuids::uuid outputClusterUUID
	 * 		UUID of output cluster
	 *	@param  int width
	 *		Width of fibre to create
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectCluster(boost::uuids::uuid inputClusterUUID,
			boost::uuids::uuid outputClusterUUID, int fibreWidth);

	/**
	 * Connect clusters specified by uuids with a fibre of width
	 *
	 * @param boost::uuids::uuid clusterUUID
	 * 		UUID of  cluster to connect to fibre
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *	@param  int width
	 *		Width of fibre to create
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectCluster(boost::uuids::uuid clusterUUID, const Fibre::FibreType & type,
			int fibreWidth);

	/**
	 * Helper access function for specialised connection
	 *
	 *	@param boost::uuids::uuid
	 *		PatternChannel to map the fibre to
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectPrimaryInputCluster(boost::uuids::uuid patchanid,
			boost::uuids::uuid clusterUUID);

	/**
	 * Helper access function for specialised connection
	 *
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *	@param  int width
	 *		Width of fibre to create
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectPrimaryInputCluster(boost::uuids::uuid clusterUUID, int fibreWidth);

	/**
	 * Helper access function for specialised connection
	 *
	 *	@param boost::uuids::uuid
	 *		PatternChannel to map the fibre to
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectPrimaryOutputCluster(boost::uuids::uuid patchanid,
			boost::uuids::uuid clusterUUID);

	/**
	 * Helper access function for specialised connection
	 *
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *	@param  int width
	 *		Width of fibre to create
	 *	@param boost::uuids::uuid
	 *		PatternChannel to map the fibre to
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectPrimaryOutputCluster(boost::uuids::uuid clusterUUID, int fibreWidth);

	/**
	 * Helper access function for specialised connection
	 *
	 * @param const Fibre::FibreType & type
	 * 		Type of fibre connection to make
	 *	@param  int width
	 *		Width of fibre to create
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	virtual boost::shared_ptr<Fibre> connectLoopbackCluster(boost::uuids::uuid clusterUUID, int fibreWidth);

	/**
	 * Load in the pattern channels from a filename of a pattern dataset
	 *
	 * @param std::string
	 * 	The full path filename of the pattern data set
	 */
	virtual void loadChannels(const std::string & ifstr);

	/**
	 * Get the clusters in this bundle
	 *
	 * @return ClusterMap
	 * 	The map of clusters in this bundle
	 */
	const ClusterMap & getClusters() const;

	/**
	 * Get the mutable clusters in this bundle
	 *
	 * @return ClusterMap
	 * 	The mutable map of clusters in this bundle
	 */
	const FibreMap & getFibres() const;

	/**
	 * Get the input fibres of this bundle
	 *
	 * @return FibreMap
	 * 	The map of input fibres of this bundle
	 */
	const FibreMap & getInputFibres() const;

	/**
	 * Get the mutable input fibres of this bundle
	 *
	 * @return FibreMap
	 * 	The map of mutable input fibres of this bundle
	 */
	FibreMap & getMutableInputFibres();

	/**
	 * Get the output fibres of this bundle
	 *
	 * @return FibreMap
	 * 	The map of output fibres of this bundle
	 */
	const FibreMap & getOutputFibres() const;

	/**
	 * Get the mutable output fibres of this bundle
	 *
	 * @return FibreMap
	 * 	The map of mutable output fibres of this bundle
	 */
	FibreMap & getMutableOutputFibres();

	/**
	 * Get the input pattern channels of this bundle
	 *
	 * @return PatternChannelMap
	 * 	The map of input pattern channels of this bundle
	 */
	const state::PatternChannelMap & getInputChannelsMap() const;

	/**
	 * Get the output pattern channels of this bundle
	 *
	 * @return PatternChannelMap
	 * 	The map of output pattern channels of this bundle
	 */
	const state::PatternChannelMap & getOutputChannelsMap() const;

	/**
	 * Get the map of fibres to their associated pattern channels
	 *
	 * @return std::map<boost::uuids::uuid, boost::uuids::uuid>
	 * 	The map of fibres to their associated pattern channels
	 */
	const std::map<boost::uuids::uuid, boost::uuids::uuid> & getFibrePatternChannelMap() const ;

	/**
	 * Get the Statistician
	 *
	 * @return boost::shared_ptr< Statistician >
	 * 	The current statistician, null pointer if we dont have one
	 */
	const boost::shared_ptr<utilities::Statistician> getStatistician() const;
	boost::shared_ptr<utilities::Statistician> getMutableStatistician();

	/**
	 * Run a system structure check
	 *
	 * @return bool
	 * 	True if system passes all tests, false otherwise
	 */
	virtual bool checkStructure() const;

	/**
	 * Check the structure of Fibres
	 *
	 * @return bool
	 * 	True if structure tests pass, false otherwise
	 */
	virtual bool checkFibreStructure() const;

	/**
	 * Check the structure of Channels
	 *
	 * @return bool
	 * 	True if structure tests pass, false otherwise
	 */
	virtual bool checkChannelStructure() const;

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Bundle & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream & operator<<(std::ostream & os, const Bundle & bundle);

protected:

	/**
	 * Get the next patterns from channels and apply them to their mapped fibres
	 */
	virtual void updatePrimaryInputFibres();

	/**
	 * Get the patterns from primary output fibres and apply them to their
	 * mapped pattern channels
	 */
	virtual void updatePrimaryOutputFibres();

	/**
	 * Update the statistician if debugging is enabled
	 */
	virtual void updateStatistician();

private:

	/**
	 * Map of all the clusters in this bundle
	 *
	 * @var ClusterMap
	 */
	ClusterMap clusters;

	/**
	 * Map of all the fibres in this bundle
	 *
	 * @var FibreMap
	 */
	FibreMap fibres;

	/**
	 * Map of all the input channels to this bundle
	 *
	 * @var PatternChannelMap
	 */
	state::PatternChannelMap inputChannelsMap;

	/**
	 * Map of all the output channels to this bundle
	 *
	 * @var PatternChannelMap
	 */
	state::PatternChannelMap outputChannelsMap;

	/**
	 * Map of all the input fibres to the bundle
	 *
	 * @var FibreMap
	 */
	FibreMap inputFibres;

	/**
	 * Map of all the output fibres to the bundle
	 *
	 * @var FibreMap
	 */
	FibreMap outputFibres;

	/**
	 * Statistics object to generate useful info on the bundle
	 */
	boost::shared_ptr<utilities::Statistician> statistician;

	/**
	 * Mapping of fibre uuid to a corresponding pattern channel
	 *
	 * @var std::map<boost::uuids::uuid, boost::uuids::uuid>
	 */
	std::map<boost::uuids::uuid, boost::uuids::uuid> fibrePatternChannelMap;

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an input PatternChannel uuid and find the input Fibre its mapped to
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the input PatternChannel
	 *
	 * @return boost::shared_ptr<Fibre>
	 * 	The input  Fibre object the input PatternChannel with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<Fibre> getPrimaryInputFibreByChannel(const boost::uuids::uuid pattern_channel_uuid);

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an output PatternChannel uuid and find the output Fibre its mapped to
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the output PatternChannel
	 *
	 * @return boost::shared_ptr<Fibre>
	 * 	The output  Fibre object the output PatternChannel with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<Fibre> getPrimaryOutputFibreByChannel(const boost::uuids::uuid pattern_channel_uuid);

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an input Fibre uuid and find the input PatternChannel its mapped to
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the input Fibre
	 *
	 * @return boost::shared_ptr<PatternChannel>
	 * 	The input PatternChannel object the input Fibre with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<state::PatternChannel> getPrimaryInputChannelByFibre(const boost::uuids::uuid fibre_uuid);

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an output Fibre uuid and find the output PatternChannel its mapped to
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the output Fibre
	 *
	 * @return boost::shared_ptr<PatternChannel>
	 * 	The output PatternChannel object the output Fibre with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<state::PatternChannel> getPrimaryOutputChannelByFibre(const boost::uuids::uuid fibre_uuid);

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an channel uuid and find the Fibre its mapped to inside the supplied map
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the PatternChannel
	 *	@param FibreMap
	 *		The map to search for a mapping from
	 *
	 * @return boost::shared_ptr<Fibre>
	 * 	The Fibre object the PatternChannel with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<Fibre> getPrimaryFibreByChannel(const boost::uuids::uuid id, FibreMap & map);

	/**
	 * Helper method to take a uuid and find its correspondingly mapped  object
	 * Take an Fibre uuid and find the PatternChannel its mapped to inside the supplied map
	 *
	 * @param boost::uuids::uuid
	 * 	The uuid of the Fibre
	 *	@param PatternChannelMap
	 *		The map to search for a mapping from
	 *
	 * @return boost::shared_ptr<PatternChannel>
	 * 	The PatternChannel object the Fibre with this uuid is mapped to, null if not found
	 */
	boost::shared_ptr<state::PatternChannel> getPrimaryChannelByFibre(const boost::uuids::uuid id,
			state::PatternChannelMap & map);
};

}

}

#endif /* BUNDLE_H_ */
