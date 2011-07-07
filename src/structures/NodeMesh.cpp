/*
 * NodeMesh.cpp
 *
 *  Created on: 22 Jun 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#define NODEMESH_DEBUG

#include "NodeMesh.h"
#include "Cluster.h"

namespace cryomesh {

namespace structures {

const double NodeMesh::INTERPOLATED_ACTIVITY_SCALING_FACTOR = (1.0 / 100.0);
const double NodeMesh::MAX_RADIUS_FRACTION_OF_BOUNDING_BOX = (1.0 / 100.0);

NodeMesh::NodeMesh(Cluster & clus) :
	cluster(clus), decayRate(1) {
	// use clusters bounding box to generate max radius

	double max_radius = cluster.getMaxBoundingBox().getDistance(spacial::Point(0, 0, 0));
	maximumNeighbourhoodRadius(max_radius * MAX_RADIUS_FRACTION_OF_BOUNDING_BOX), this->regenerateNeighbourhoods();
}

NodeMesh::NodeMesh(Cluster & clus, double max_radius) :
	cluster(clus), maximumNeighbourhoodRadius(max_radius), decayRate(1) {
	this->regenerateNeighbourhoods();
}

NodeMesh::~NodeMesh() {
}

void NodeMesh::update() {
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::update: " << this << std::endl;
#endif
	this->regenerateActivities();
}

void NodeMesh::warpNodes() {
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::warpNodes: " << this << " -> " << neighbourhoodActivities.size() << std::endl;
#endif
	// forall in neighbourhoodActivities
	{
		std::map<boost::shared_ptr<components::Node>, double>::iterator it_neighbourhoodActivities =
				neighbourhoodActivities.begin();
		const std::map<boost::shared_ptr<components::Node>, double>::const_iterator it_neighbourhoodActivities_end =
				neighbourhoodActivities.end();
		while (it_neighbourhoodActivities != it_neighbourhoodActivities_end) {
			// generate an impulse based on the interpolated activity
			double interpolated_activity = it_neighbourhoodActivities->second;
			if (interpolated_activity > 0 || interpolated_activity < 0) {
				boost::shared_ptr<components::Impulse> interpolated_impulse(
						new components::Impulse(interpolated_activity * INTERPOLATED_ACTIVITY_SCALING_FACTOR, 1));
#ifdef NODEMESH_DEBUG
				std::cout << "NodeMesh::warpNodes: " << this << " -> "
						<< "interpolated_activity * INTERPOLATED_ACTIVITY_SCALING_FACTOR = " << interpolated_activity
						<< " * " << INTERPOLATED_ACTIVITY_SCALING_FACTOR << " = " << interpolated_activity
						* INTERPOLATED_ACTIVITY_SCALING_FACTOR << std::endl;
#endif
				it_neighbourhoodActivities->first->addImpulse(interpolated_impulse);

			}
			++it_neighbourhoodActivities;
		}
	}
}

void NodeMesh::regenerateNeighbourhoods() {
	nodeNeighbourhoodMap.clear();
	neighbourhoodActivities.clear();

	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = cluster.getNodes();
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::regenerateNeighbourhoods: " << this << " -> " << "all_nodes: " << all_nodes.size()
			<< std::endl;
#endif
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
				++it_other_nodes;
			}
			++it_all_nodes;
		}

	}
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::regenerateNeighbourhoods: " << this << " -> " << "nodeNeighbourhoodMap: "
			<< nodeNeighbourhoodMap.size() << std::endl;
#endif
}

void NodeMesh::regenerateActivities() {
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::regenerateActivities: " << this << " -> " << "nodeNeighbourhoodMap: "
			<< nodeNeighbourhoodMap.size() << std::endl;
#endif
	// for each node
	NeighbourhoodMapConstIterator it_neighbourhood = nodeNeighbourhoodMap.begin();
	const NeighbourhoodMapConstIterator it_neighbourhood_end = nodeNeighbourhoodMap.end();
	while (it_neighbourhood != it_neighbourhood_end) {
		neighbourhoodActivities[it_neighbourhood->first] = this->getInterpolatedActivity(it_neighbourhood->second);
		++it_neighbourhood;
	}
#ifdef NODEMESH_DEBUG
	std::cout << "NodeMesh::regenerateActivities: " << this << " -> " << "neighbourhoodActivities: "
			<< neighbourhoodActivities.size() << std::endl;
#endif
}

double NodeMesh::getInterpolatedActivity(
		const std::map<boost::shared_ptr<cryomesh::components::Node>, double> & all_neighbours,
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

NodeMesh::NeighbourhoodRanges NodeMesh::getNeighbourRanges() const {
	NeighbourhoodRanges ranges;
	int min_nbs = -1;
	int max_nbs = -1;
	double min_dist = -1;
	double max_dist = -1;

	NeighbourhoodMapConstIterator it_nodeNeighbourhoodMap = nodeNeighbourhoodMap.begin();
	const NeighbourhoodMapConstIterator it_nodeNeighbourhoodMap_end = nodeNeighbourhoodMap.end();
	while (it_nodeNeighbourhoodMap != it_nodeNeighbourhoodMap_end) {
		int count = 0;
		std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_neighbours =
				it_nodeNeighbourhoodMap->second.begin();
		const std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_neighbours_end =
				it_nodeNeighbourhoodMap->second.end();
		while (it_neighbours != it_neighbours_end) {
			if (min_dist < 0) {
				min_dist = it_neighbours->second;
			}
			min_dist = std::min(min_dist, it_neighbours->second);
			max_dist = std::max(max_dist, it_neighbours->second);
			++count;
			++it_neighbours;
		}
		if (min_nbs < 0) {
			min_nbs = count;
		}
		min_nbs = std::min(min_nbs, count);
		max_nbs = std::max(max_nbs, count);
		++it_nodeNeighbourhoodMap;
	}
	ranges.minimumNeighbourCount = min_nbs;
	ranges.maximumNeighbourCount = max_nbs;
	ranges.minimumNeighbourDistance = min_dist;
	ranges.maximumNeighbourDistance = max_dist;
	return ranges;
}

const NeighbourhoodMap & NodeMesh::getNodeNeighbourhoodMap() const {
	return nodeNeighbourhoodMap;
}
const std::map<boost::shared_ptr<components::Node>, double> & NodeMesh::getNeighbourhoodActivities() const {
	return neighbourhoodActivities;
}

std::ostream & NodeMesh::printNeighbourhoods(std::ostream & os) const {
	NeighbourhoodMapConstIterator it_nodeNeighbourhoodMap = nodeNeighbourhoodMap.begin();
	const NeighbourhoodMapConstIterator it_nodeNeighbourhoodMap_end = nodeNeighbourhoodMap.end();
	while (it_nodeNeighbourhoodMap != it_nodeNeighbourhoodMap_end) {
		os << it_nodeNeighbourhoodMap->first->getUUIDSummary() << "->";

		std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_neighbours =
				it_nodeNeighbourhoodMap->second.begin();
		const std::map<boost::shared_ptr<cryomesh::components::Node>, double>::const_iterator it_neighbours_end =
				it_nodeNeighbourhoodMap->second.end();
		while (it_neighbours != it_neighbours_end) {
			os << "(" << it_neighbours->first->getUUIDSummary() << ", " << it_neighbours->second << ") ";
			++it_neighbours;
		}
		++it_nodeNeighbourhoodMap;
		if (it_nodeNeighbourhoodMap != it_nodeNeighbourhoodMap_end) {
			os << std::endl;
		}
	}
	return os;
}

std::ostream & NodeMesh::printNeighbourhoodActivities(std::ostream & os) const {
	// forall in neighbourhoodActivities
	{
		std::map<boost::shared_ptr<components::Node>, double>::const_iterator it_neighbourhoodActivities =
				neighbourhoodActivities.begin();
		const std::map<boost::shared_ptr<components::Node>, double>::const_iterator it_neighbourhoodActivities_end =
				neighbourhoodActivities.end();
		while (it_neighbourhoodActivities != it_neighbourhoodActivities_end) {
			os << it_neighbourhoodActivities->first->getUUIDSummary() << ", " << it_neighbourhoodActivities->second;

			++it_neighbourhoodActivities;
			if (it_neighbourhoodActivities != it_neighbourhoodActivities_end) {
				os << std::endl;
			}
		}
	}
	return os;
}

std::ostream& operator<<(std::ostream & os, const NodeMesh & obj) {
	os << "NodeMesh:" << std::endl;
	os << obj.printNeighbourhoods(os) << std::endl;
	os << obj.printNeighbourhoodActivities(os);
	return os;
}

}//NAMESPACE

}//NAMESPACE
