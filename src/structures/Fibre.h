/*
 * Fibre.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef FIBRE_H_
#define FIBRE_H_

#include "components/ConnectionMap.h"
#include "common/Connector.h"
#include "state/Pattern.h"
#include <boost/shared_ptr.hpp>
#include "common/Tagged.h"

namespace cryomesh {

namespace structures {

class Cluster;
/**
 * A Fibre is a collection of connections that connect one structure to another. For example,
 * two clusters.
 */
class Fibre: public common::Tagged {
public:
	enum ClusterConnectionType {
		NullCluster, InputCluster, OutputCluster, LoopbackCluster
	};

	enum FibreType {
		NullFibre, // The null Fibre
		IntermediateFibre, // intermedate fibre, [ clusterin->clusterout ]
		PrimaryInputFibre, // primary input, [ ->cluster ]
		PrimaryOutputFibre, // primary output, [ cluster -> ]
		LoopbackFibre
	// Loop back, [cluster1->cluster1 ]
	};

	/**
	 * Construct a fibre between two clusters with width
	 */
	Fibre(boost::shared_ptr<Cluster> inputCluster, boost::shared_ptr<Cluster> outputCluster, int width);

	/**
	 * Construct a primary fibre with width
	 *
	 * @param boost::shared_ptr<Cluster> cluster
	 * 		Cluster to connect to fibre
	 * @param  const FibreType & type
	 * 		Type of fibre connection to make
	 *	@param  int width
	 *		Width of fibre to create
	 *
	 * @return
	 * 		The new fibre created, possible null
	 */
	Fibre(boost::shared_ptr<Cluster> cluster, const FibreType & type, int width);

	/**
	 * Default destructor
	 */
	virtual ~Fibre();

	/**
	 * Update all elements
	 */
	virtual void update();

	/**
	 * Trigger a random pattern with percentage of the connections to fire
	 *
	 * @param double
	 * 		Fraction of connection to trigger randomly
	 */
	void trigger(double percentage);

	/**
	 * Send impulses to the connections based on a complete trigger of all of them
	 */
	virtual void trigger();

	/**
	 * Send impulses to the connections based on a pattern
	 *
	 *	@param state::Pattern & pattern
	 *		The pattern to use to create impulses and send to connections
	 */
	virtual void trigger(const state::Pattern & pattern);

	/**
	 * Send impulses to the connections
	 *
	 *	@param const std::vector<boost::shared_ptr< components::Impulse > > & triggerImpulses
	 *		The impulses to send to connections
	 */
	virtual void trigger( std::vector<boost::shared_ptr<components::Impulse> > & triggerImpulses);

	/**
	 * Get the connector
	 *
	 * @return common::Connector<Fibre, Cluster> &
	 * 		The connector object
	 */
	const common::Connector<Fibre, Cluster> & getConnector() const;

	/**
	 * Get the mutable connector
	 *
	 * @return common::Connector<Fibre, Cluster> &
	 * 		The connector object
	 */
	common::Connector<Fibre, Cluster> & getMutableConnector();

	/**
	 * Get the connections
	 *
	 * @return const components::ConnectionMap &
	 * 		The connection map for this Fibre
	 */
	const components::ConnectionMap & getConnections() const;

	const FibreType & getType() const;

	/**
	 * Return type of connection to cluster specified, null if none
	 *
	 * @param boost::shared_ptr<Cluster> cluster
	 * 		Check connection to this cluster
	 *
	 * @return const ClusterConnectionType &
	 * 		Connection type to cluster, Null if none
	 */
	const ClusterConnectionType isConnected(const boost::shared_ptr<Cluster> & cluster) const;

	/**
	 * Get width (number of connections) of fibre
	 *
	 * @return unsigned int
	 * 		Width of fibre
	 */
	unsigned int getWidth()const;

	/**
	 * Get current pattern for firing state of input nodes to the fibre
	 */
	boost::shared_ptr< state::Pattern >  getInputNodesPattern() const;
	/**
	 * Get the activity pattern of the Fibre, 0 for no activity, 1 otherwise
	 *
	 * @return Pattern
	 */

	/**
		 * To stream operator
		 *
		 *	@param std::ostream & os
		 *		The output stream
		 *	@param const Fibre & obj
		 *		The object to stream
		 *
		 *	@return std::ostream &
		 *		The output stream
		 */
		friend std::ostream& operator<<(std::ostream & os, const Fibre & obj);
protected:

	/**
	 * Set the type of this fibre
	 *
	 * @param const FibreType & tp
	 * 		The type of this fibre
	 */
	virtual void setType(const FibreType & tp);

	/**
	 * Reset and create a number of connections in this Fibre
	 *
	 * @param int number
	 * 		Number of connections to create
	 */
	virtual void createConnections(int number);

	/**
	 * Disconnect all the connections from clusters
	 */
	virtual void disconnectAllConnections();

	/**
	 * Connect all the connections in this Fibre to a cluster of a specified type
	 *
	 * @param boost::shared_ptr<Cluster> cluster
	 * 		The cluster to connect to
	 * @param ClusterConnectionType type
	 * 		The type of cluster we're connecting to
	 */
	virtual void connectAllConnections(boost::shared_ptr<Cluster> cluster, ClusterConnectionType type);

private:
	/**
	 * Connector object for this Fibre
	 *
	 * @var common::Connector<Fibre, Cluster>
	 */
	common::Connector<Fibre, Cluster> connector;

	/**
	 * ConnectionMap for the connections of this fibre
	 *
	 * @var components::ConnectionMap
	 */
	components::ConnectionMap connections;

	/**
	 * The type of this Fibre
	 *
	 * @var FibreType
	 */
	FibreType fibreType;
};

}

}

#endif /* FIBRE_H_ */
