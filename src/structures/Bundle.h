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

namespace cryomesh {

namespace structures {

/**
 * A Bundle is the collection of clusters and fibres, it represents the system as a whole
 */
class Bundle {
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
	virtual boost::shared_ptr<Fibre> connectPrimaryInputCluster(boost::uuids::uuid clusterUUID, int fibreWidth, boost::uuids::uuid patchanid);

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
	virtual boost::shared_ptr<Fibre> connectPrimaryOutputCluster(boost::uuids::uuid clusterUUID, int fibreWidth, boost::uuids::uuid patchanid);


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
	 * Load in the pattern channels from a filename and create equivalent
	 * sized fibres to go with them, tracking the mapping of the pattern
	 * channel to the fibre
	 */
	virtual void loadChannels(const std::string & ifstr);


	const ClusterMap & getClusters() const;
	const FibreMap & getFibres() const;

	const FibreMap & getInputFibres() const;
	const FibreMap & getOutputFibres() const;

	const state::PatternChannelMap & getInputChannelsMap() const;
	const state::PatternChannelMap & getOutputChannelsMap() const;
	const std::map<boost::uuids::uuid, boost::uuids::uuid> & getFibrePatternChannelMap()const ;

	friend std::ostream & operator<<(std::ostream & os, const Bundle & bundle);

protected:


private:

	ClusterMap clusters;
	FibreMap fibres;

	state::PatternChannelMap inputChannelsMap;
	state::PatternChannelMap outputChannelsMap;

	FibreMap inputFibres;
	FibreMap outputFibres;

	/**
	 * Mapping of fibre uuid to a corresponding pattern channel
	 */
	std::map<boost::uuids::uuid, boost::uuids::uuid> fibrePatternChannelMap;
};

}

}

#endif /* BUNDLE_H_ */
