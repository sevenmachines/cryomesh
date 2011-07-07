/*
 * NodeMesh.h
 *
 *  Created on: 22 Jun 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef NODEMESH_H_
#define NODEMESH_H_

#include "components/Node.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace structures {

class Cluster;

/**
 * Typedef to simplify neighbourhood map structure
 */
typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> > NeighbourhoodMap;
/**
 * Typdef for iterator to neighbourhood map
 */
typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> >::const_iterator NeighbourhoodMapConstIterator;
/**
 * Mesh of nodes and their neighbouring nodes and distances
 */
class NodeMesh {
public:

/**
 * Struct to capture some statistics data on a nodes neighbourhood
 */
	struct NeighbourhoodRanges {
		int minimumNeighbourCount;
		int maximumNeighbourCount;
		double minimumNeighbourDistance;
		double maximumNeighbourDistance;
	};

/**
 * Stype to use when interpolating values
 */
	enum InterpolationStyle {
		INVERSE_R, INVERSE_R2
	};

	/**
		 * Constructor to create a node mesh from  a cluster
		 *
		 * @param Cluster
		 * 	The cluster associated with this node mesh
		 */
		NodeMesh(Cluster & clus);

	/**
	 * Constructor to create a node mesh from at minimum, a cluster
	 * and an optional maximum neighbour radius
	 *
	 * @param Cluster
	 * 	The cluster associated with this node mesh
	 * @param double
	 * 	Maximum radius cutoff point for neighbourhood distance
	 */
	NodeMesh(Cluster & clus, double max_radius);

	/**
	 * Default destructor
	 */
	virtual ~NodeMesh();

	/**
	 * Per cycle update calls
	 */
	void update();

	/**
	 * Merge the interpolated activities with the actual node activities
	 */
	void warpNodes();

	/**
	 * Regenerate the neighbourhood nodes and distances
	 */
	void regenerateNeighbourhoods();

	/**
	 * Recalculate the applied interpolated activity at all nodes
	 * from their neighbours
	 */
	void regenerateActivities();

	/**
	 * Return a pair of pairs. the first representing the min/max
	 * of neighbour counts, the second the min/max of distances
	 *
	 * @return NeighbourhoodRanges
	 * 	min/max of neighbour counts and min/max of distances
	 */
	NeighbourhoodRanges getNeighbourRanges() const;

	/**
	 * Get the neighbourhood map
	 *
	 * @return NeighbourhoodMap
	 * 	The neighbourhood map
	 */
	const NeighbourhoodMap & getNodeNeighbourhoodMap() const;

	/**
	 * Get the neighbourhood activities
	 *
	 * @return std::map<boost::shared_ptr<components::Node>, double>
	 * 	The neighbourhood activities
	 */
	const std::map<boost::shared_ptr<components::Node>, double> & getNeighbourhoodActivities() const;

	/**
	 * Print the neighbourhood map
	 *
	 * @param std::ostream
	 * 	The output stream
	 *
	 * @return std::ostream
	 * 	The output stream
	 */
	std::ostream & printNeighbourhoods(std::ostream & os) const;

	/**
	 * Print the neighbourhood activities
	 *
	 * @param std::ostream
	 * 	The output stream
	 *
	 * @return std::ostream
	 * 	The output stream
	 */
	std::ostream & printNeighbourhoodActivities(std::ostream & os) const;

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const NodeMesh & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const NodeMesh & obj);

protected:
	/**
	 * Use a list of neighbours and their distances to generated an interpolated activity value at
	 * the central node
	 *
	 * @param std::map<boost::shared_ptr<cryomesh::components::Node>, double>
	 * 	List of all the neighbour nodes and their distances to their central node
	 * @param InterpolationStyle
	 * 	Which method to use to interpolate the central activity
	 *
	 * @return double
	 * 	The interpolated activity
	 */
	double getInterpolatedActivity(
			const std::map<boost::shared_ptr<cryomesh::components::Node>, double> & all_neighbours,
			const InterpolationStyle style = INVERSE_R) const;

	/**
	 * Get the spacial decay rate for activities
	 *
	 * @return double
	 * 	The spacial decay rate
	 */
	double getDecayRate() const;

	/**
	 * Fraction of a clusters  bounding box to use as a max radius
	 *
	 * @var double
	 */
	static const double MAX_RADIUS_FRACTION_OF_BOUNDING_BOX;

	/**
		 * Scaling factor for applying interpolated activities to actual activities
		 *
		 * @var double
		 */
		static const double INTERPOLATED_ACTIVITY_SCALING_FACTOR;


private:
	/**
	 * Map of Nodes to other nodes within their neighbourhood and the distances to them
	 *
	 * @var NeighbourhoodMap
	 */
	NeighbourhoodMap nodeNeighbourhoodMap;

	/**
	 * Map of each node to its interpolated activity applied by its neighbours
	 *
	 * @var std::map<boost::shared_ptr<components::
	 */
	std::map<boost::shared_ptr<components::Node>, double> neighbourhoodActivities;

	/**
	 * The cluster that is attached to this node mesh
	 *
	 *	@var Cluster
	 */
	Cluster & cluster;

	/**
	 * The maximum radius to use as a cut off point to calculate neighbours
	 *
	 * @var double
	 */
	double maximumNeighbourhoodRadius;

	/**
	 * The spacial decay rate to use for interpolating activities
	 *
	 * @var double
	 */
	const double decayRate;
};

}

}

#endif /* NODEMESH_H_ */
