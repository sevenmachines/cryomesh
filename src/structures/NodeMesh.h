/*
 * NodeMesh.h
 *
 *  Created on: 22 Jun 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef NODEMESH_H_
#define NODEMESH_H_

#include "components/Node.h"
#include "Cluster.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace structures {

typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> > NeighbourhoodMap;
typedef std::map<boost::shared_ptr<cryomesh::components::Node>, std::map<boost::shared_ptr<cryomesh::components::Node>,
		double> >::const_iterator NeighbourhoodMapConstIterator;
/**
 * Mesh of nodes and their neighbouring nodes and distances
 */
class NodeMesh {
public:

	enum InterpolationStyle{
		INVERSE_R, INVERSE_R2
	};

	NodeMesh(Cluster & clus, double max_radius = 1);
	virtual ~NodeMesh();
	void regenerateNeighbourhoods();
	void regenerateActivities();


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
