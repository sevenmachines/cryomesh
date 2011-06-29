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

typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> > NeighbourhoodMap;
typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> >::const_iterator NeighbourhoodMapConstIterator;
/**
 * Mesh of nodes and their neighbouring nodes and distances
 */
class NodeMesh {
public:

	struct NeighbourhoodRanges{
		int minimumNeighbourCount;
		int maximumNeighbourCount;
		double minimumNeighbourDistance;
		double maximumNeighbourDistance;
	};
	enum InterpolationStyle{
		INVERSE_R, INVERSE_R2
	};

	NodeMesh(Cluster & clus, double max_radius = 1);
	virtual ~NodeMesh();
	void update();
	void regenerateNeighbourhoods();
	void regenerateActivities();

	/**
	 * Return a pair of pairs. the first representing the min/max
	 * of neighbour counts, the second the min/max of distances
	 *
	 * @return NeighbourhoodRanges
	 * 	min/max of neighbour counts and min/max of distances
	 */
	NeighbourhoodRanges getNeighbourRanges() const;

	std::ostream & printNeighbourhoods(std::ostream & os)const;
	std::ostream & printNeighbourhoodActivities(std::ostream & os)const;

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
	double getInterpolatedActivity(const std::map<boost::shared_ptr<cryomesh::components::Node>, double> & all_neighbours, const InterpolationStyle style = INVERSE_R)const;
	double getDecayRate()const;

private:
	NeighbourhoodMap nodeNeighbourhoodMap;
	std::map< boost::shared_ptr< components::Node >, double> neighbourhoodActivities;

	Cluster & cluster;
	double maximumNeighbourhoodRadius;

	const double decayRate;
};

}

}

#endif /* NODEMESH_H_ */
