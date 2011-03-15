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
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>

#include <components/Mesh.h>
#include <components/Node.h>
#include <components/Connection.h>

#include "common/Connector.h"
#include "common/Tagged.h"

namespace cryomesh {

namespace structures {

class Fibre;

/**
 * A Cluster is a collection of self-contained nodes and connections along with an associated Mesh,
 * that can be connected up to one another
 */
class Cluster: public common::Tagged {
public:

	/**
	 * Enum to force value of a parameter
	 */
	enum ValueTypeSpecifier {
		AsIncrement, AsDecrement, AsMinumum, AsAbsolute
	};

	/**
	 * Constructor
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

	const common::Connector<Cluster, Fibre> & getConnector() const;
	common::Connector<Cluster, Fibre> & getMutableConnector();

	const components::NodeMap & getNodeMap()const;
	components::NodeMap & getMutableNodeMap();

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
	components::NodeMap nodes;
	components::ConnectionMap connections;
	components::Mesh mesh;

	common::Connector<Cluster, Fibre> connector;

};

}

}

#endif /* CLUSTER_H_ */
