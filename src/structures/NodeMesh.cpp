/*
 * NodeMesh.cpp
 *
 *  Created on: 22 Jun 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "NodeMesh.h"

namespace cryomesh {

namespace structures {

NodeMesh::NodeMesh(Cluster & clus, double max_radius) :
	cluster(clus), maximumNeighbourhoodRadius(max_radius), decayRate(1) {

}

NodeMesh::~NodeMesh() {
	// TODO Auto-generated destructor stub
}

void NodeMesh::regenerateNeighbourhoods() {

	nodeNeighbourhoodMap.clear();
	neighbourhoodActivities.clear();

	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = cluster.getNodes();
	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {

			neighbourhoodActivities[it_all_nodes->second] = 0.0;

			std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_other_nodes =
					it_all_nodes;
			++it_other_nodes;

			while (it_other_nodes != it_all_nodes_end) {
				// get distance
				double dist = it_all_nodes->second->getPosition().getDistance(it_other_nodes->second->getPosition());
				if (dist < maximumNeighbourhoodRadius) {
					nodeNeighbourhoodMap[it_all_nodes->second][it_other_nodes->second] = dist;
					nodeNeighbourhoodMap[it_other_nodes->second][it_all_nodes->second] = dist;
				}
			}
			++it_other_nodes;
		}
		++it_all_nodes;
	}

}

void NodeMesh::regenerateActivities() {
	// for each node
	NeighbourhoodMapConstIterator it_neighbourhood = nodeNeighbourhoodMap.begin();
	const NeighbourhoodMapConstIterator it_neighbourhood_end = nodeNeighbourhoodMap.end();
	while (it_neighbourhood != it_neighbourhood_end){
		neighbourhoodActivities[it_neighbourhood->first] = this->getInterpolatedActivity(it_neighbourhood->second);
		++it_neighbourhood;
	}

}

double NodeMesh::getInterpolatedActivity(const std::map<boost::shared_ptr<cryomesh::components::Node>, double> & all_neighbours,
		const InterpolationStyle style) const {
	const double ZERO_DELTA = 0.00001;
	double activity_sum = 0;

	// forall in all_neighbours
	{
		std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_all_neighbours =
				all_neighbours.begin();
		const std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_all_neighbours_end =
				all_neighbours.end();
		while (it_all_neighbours != it_all_neighbours_end) {

			double nb_activity = it_all_neighbours->first->getActivity();
			double nb_distance = it_all_neighbours->second;

			double interpolated_act;
			if ((nb_activity > ZERO_DELTA) || (nb_activity < -ZERO_DELTA)) {
				if (style == InterpolationStyle::INVERSE_R) {
					double remove_activity = (this->getDecayRate() * nb_distance);
					if (nb_activity >= 0) {
						interpolated_act = std::max(0.0, (nb_activity - remove_activity));
					} else {
						interpolated_act = std::min(0.0, (nb_activity + remove_activity));
					}
				} else if (style == InterpolationStyle::INVERSE_R2) {
					double remove_activity = (this->getDecayRate() * nb_distance);
					if (nb_activity >= 0) {
						interpolated_act = nb_activity - (remove_activity * remove_activity);
					} else {
						interpolated_act = nb_activity + (remove_activity * remove_activity);

					}
				} else {
					std::cout << "NodeMesh::getInterpolatedActivity: " << "ERROR: Unknown Interpolation style"
							<< std::endl;
					assert(false);
				}
				if ((nb_activity > 0 && interpolated_act < 0) || (nb_activity < 0 && interpolated_act > 0)) {
					interpolated_act = 0;
				}
				activity_sum += interpolated_act;
			}

			++it_all_neighbours;
		}
	}

	return activity_sum;
}

double NodeMesh::getDecayRate() const {
	return decayRate;
}
}//NAMESPACE

}//NAMESPACE
