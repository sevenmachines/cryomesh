/*
 * Mesh.cpp
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "Mesh.h"
#include "components/Node.h"
#include "components/Impulse.h"
#include "components/ImpulseCollection.h"
#include "Cluster.h"

using namespace cryomesh::components;
using namespace cryomesh::spacial;
namespace cryomesh {

namespace structures {

//DEPRECATED
/*
 boost::shared_ptr<Mesh> Mesh::mesh;

 boost::shared_ptr<Mesh> Mesh::getMesh() {
 if (mesh == 0) {
 mesh = boost::shared_ptr<Mesh>(new Mesh());
 }
 return mesh;
 }
 */

Mesh::Mesh(Cluster & clus) :
	cluster(clus), DEFAULT_MESH_GRANULARITY(10), DEFAULT_BLEND_FORCE(0.1) {
	// make us the same side as the max bounding box
	// Generate activity grid from custer
	Point bounding_box = cluster.getMaxBoundingBox();
	grid = boost::shared_ptr<spacial::ActivityGrid>(new spacial::ActivityGrid(bounding_box, DEFAULT_MESH_GRANULARITY));
	this->update();
}

Mesh::~Mesh() {
}

void Mesh::update() {
	// go over all nodes in cluster, affecting the mesh activities
	const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> > & all_nodes = cluster.getNodes();
	// forall in all_nodes
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes =
				all_nodes.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_all_nodes_end =
				all_nodes.end();
		while (it_all_nodes != it_all_nodes_end) {
			const Node & temp_node = *(it_all_nodes->second);
			const Point & temp_point = temp_node.getPosition();
			// get current activity at point
			double current_activity = grid->getInterpolatedActivity(temp_point, 1);
			double new_activity = temp_node.getActivity();
#ifdef MESH_DEBUG
			std::cout << "Mesh::update: " << "grid->applyPointActivityToGrid(" << temp_point << ", (" << new_activity
					<< " - " << current_activity << "));" << std::endl;
#endif
			grid->applyPointActivityToGrid(temp_point, (new_activity - current_activity));
			++it_all_nodes;
		}
	}
}

Node & Mesh::warp(Node & node) {
	// get activity value at grid point near node
	double interpolated = grid->getInterpolatedActivity(node.getPosition());
	// blend the activities
	double warped_activity = this->getBlendedActivity(node.getActivity(), interpolated, Mesh::BLEND_LINEAR,
			DEFAULT_BLEND_FORCE);
	node.setActivity(warped_activity);
	return node;
}

ImpulseCollection & Mesh::warp(ImpulseCollection & impulseCollection) {
	//TODO Mesh::warp (ImpulseCollection&)
	std::cout << "Mesh::warp(ImpulseCollection &): " << "TODO" << std::endl;
	return impulseCollection;
}

Impulse & Mesh::warp(Impulse & impulse) {
	//TODO Mesh::warp (Impulse&)
	std::cout << "Mesh::warp(Impulse &): " << "TODO" << std::endl;
	return impulse;
}

double Mesh::getBlendedActivity(const double first_activity, const double second_activity,
		const BlendingMethod blending_method, double force) {
	common::Maths::clamp(force, 0.0, 1.0);
	double result;
	if (blending_method == Mesh::BLEND_LINEAR) {
		result = first_activity + (force * (second_activity - first_activity));
	} else {
		result = first_activity + (force * (second_activity - first_activity));
	}
	return result;
}

const Cluster & Mesh::getCluster() const {
	return cluster;
}

const boost::shared_ptr<spacial::ActivityGrid> Mesh::getActivityGrid() const {
	return grid;
}

}//NAMESPACE

}//NAMESPACE
