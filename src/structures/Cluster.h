/*
 * Cluster.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include "components/NodeMap.h"
#include "components/ConnectionMap.h"
#include "common/Spacial.h"

#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>

#include <components/Mesh.h>
#include <components/Node.h>
#include <components/Connection.h>

#include "common/Connector.h"
#include "common/Tagged.h"
#include "common/Debuggable.h"

namespace cryomesh {

namespace structures {

class Fibre;

/**
 * A Cluster is a collection of self-contained nodes and connections along with an associated Mesh,
 * that can be connected up to one another
 */
class Cluster: public common::Tagged, public common::Spacial , public common::Debuggable{
public:

	/**
	 * Enum to force value type of a parameter
	 *
	 */
	enum ValueTypeSpecifier {
		AsIncrement, AsDecrement, AsMinumum, AsAbsolute
	};

	/**
	 * Default constructor
	 */
	Cluster();

	/**
	 * Constructor with number of nodes and connectivity
	 *
	 * @param int nodeCount
	 * 		Number of nodes to make
	 * @param int connectivity
	 * 		Connectivity to start with
	 */
	Cluster(int nodeCount, int connectivity);

	/**
	 * Destructor
	 */
	virtual ~Cluster();

	/**
	 * Update all elements
	 */
	void update();
	void updateEnergy(double total_energy);

	void warpMesh() ;

	/**
	 * Create a number of nodes in the cluster
	 *
	 * @param int number
	 * 		Number of nodes to create
	 */
	virtual void createNodes(const int number = 1);

	/**
	 * Create connections between all nodes
	 *
	 * @param int connectivity
	 * 		The number of connections between each node
	 */
	virtual void createConnectivity(const int connectivity = 1);

	/**
	 * Get all nodes
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr< components::Node > >
	 * 		Return all nodes
	 */
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & getNodes() const;

	/**
	 * Get all connections
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr< components::Connection > >
	 * 		Return all Connections
	 */
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & getConnections() const;

	/**
	 * Get the connector for this Cluster
	 *
	 * @return common::Connector<Cluster, Fibre>
	 * 	The connector for this Cluster
	 */
	const common::Connector<Cluster, Fibre> & getConnector() const;

	/**
	 * Get the mutable connector for this Cluster
	 *
	 * @return common::Connector<Cluster, Fibre>
	 * 	The mutable onnector for this Cluster
	 */
	common::Connector<Cluster, Fibre> & getMutableConnector();

	/**
	 * Get the NodeMap for this Cluster
	 *
	 * @return components::NodeMap
	 * 	The NodeMap for this Cluster
	 */
	const components::NodeMap & getNodeMap() const;

	/**
	 * Get the mutable NodeMap for this Cluster
	 *
	 * @return components::NodeMap
	 * 	The mutable NodeMap for this Cluster
	 */
	components::NodeMap & getMutableNodeMap();

	/**
	 * Get the total fired nodes in this cluster currently
	 *
	 *@param int
	 *		Set >0 for only positive triggered nodes, <0 for negative, 0 for all (default)
	 *
	 *@return int
	 *		The total count of currently triggered nodes
	 */
	int getTriggeredNodeCount(const int indicator=0) const;

	/**
	 * Get the total active nodes in this cluster currently
	 *
	 *@param int
	 *		Set >0 for only positive active nodes, <0 for negative, 0 for all (default)
	*
	 *@return int
	 *		The total count of currently active nodes
	 */
	int getActiveNodeCount(const int indicator=0) const;

	/**
	 * Get the total live nodes in this cluster currently, ie those with at least
	 * one impulse
	*
	 *@return int
	 *		The total count of currently live nodes
	 */
	int getLiveNodeCount() const;

	double getEnergy()const;
	void setEnergy(double d);

	virtual void enableDebug(bool b) ;
	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Cluster & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Cluster & obj);

	static const double SELF_CONNECTED_NODES_FRACTION;

protected:

	/**
	 * Update connectivity so that each node has at least param number of connections
	 *
	 * @param int connectivity
	 * 		The least connectivity to ensure
	 */
	virtual void updateConnectivity(const int connectivity, ValueTypeSpecifier asValue = AsIncrement);

	/**
	 * Create connection between an input node and output node
	 *
	 * @param boost::shared_ptr<Node> nodeStart
	 * 		Input node to connect
	 * @param boost::shared_ptr<Node> nodeEnd
	 * 		Output node to connect
	 * @param int connecticity
	 * 		Number of connections to create between start and end nodes
	 */
	virtual void createConnection(boost::shared_ptr<components::Node> nodeStart,
			boost::shared_ptr<components::Node> nodeEnd, int connectivity = 1);

private:
	double energy;

	/**
	 * All the nodes of this cluster stored in a NodeMap
	 *
	 * @var NodeMap
	 */
	components::NodeMap nodes;

	/**
	 * All the connections of this cluster stored in a ConnectionMap
	 *
	 * @var ConnectionMap
	 */
	components::ConnectionMap connections;

	/**
	 * The underlying mesh of this cluster
	 *
	 * @var Mesh
	 */
	components::Mesh mesh;

	/**
	 * The Cluster to Fibre connector for this cluster
	 *
	 * @var Connector
	 */
	common::Connector<Cluster, Fibre> connector;

};

}

}

#endif /* CLUSTER_H_ */
