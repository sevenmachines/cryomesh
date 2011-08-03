/**
 * Mesh.h
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef MESH_H_
#define MESH_H_

#include "common/Cycle.h"
#include "spacial/ActivityGrid.h"
#include <boost/shared_ptr.hpp>

namespace cryomesh {
namespace components {
// Forward declare to avoid cyclical header dependencies
class Impulse;
class ImpulseCollection;
class Node;
}
namespace structures {
class Cluster;

/**
 * Mesh is the fabric of connection space and warps and is warped by it.
 *
 * The Mesh is an overlying structure on top of Connections space
 * that can be used to warp the underlying space, Impulses, Connections,
 * Nodes, etc based on spacial criteria on any other criteria that can
 * be applied to the underlying data objects
 */
class Mesh {
public:

	enum BlendingMethod {
		BLEND_LINEAR
	};
	/**
	 * Constructor
	 *
	 * 		Constructor for Mesh. Inaccessible to force singleton class
	 */
	Mesh(Cluster & clus);

	/**
	 * Copy Constructor
	 *
	 * 		Overridden Copy Contructor for Mesh. Inaccessible to force singleton class
	 *
	 * @param Mesh
	 * 		Object to Copy Construct from
	 */
	Mesh(const Mesh &);

	/**
	 * Destructor
	 */
	virtual ~Mesh();

	/**
	 * Update mesh from cluster
	 */
	void update();

	/**
	 * Warp the a Node using the mesh
	 *
	 * 		This function will use any values of the node, such as position in space
	 * for example, to apply a warp to the node. In practice this might be to suppress or
	 * increase the activity threshold, or to scale the activites at that node in some way.
	 * Note that this is an permanent change, ie, the node is warped 'in place'
	 *
	 * @param Node & node
	 * 		The node to be warped
	 *
	 * @return Node &
	 * 		The warped node
	 */
	components::Node & warp(components::Node & node);

	/**
	 * Warp an ImpulseCollection using the mesh
	 *
	 * @param ImpulseCollection & ImpulseCollection
	 *
	 * @return ImpulseCollection &
	 * 		The warped collection
	 */
	components::ImpulseCollection & warp(components::ImpulseCollection & impulseCollection);

	/**
	 * Warp an Impulse using the mesh
	 *
	 * @param Impulse & Impulse
	 *
	 * @return Impulse &
	 * 		The warped Impulse
	 */
	cryomesh::components::Impulse & warp(cryomesh::components::Impulse & impulse);

	const Cluster & getCluster() const;
	const boost::shared_ptr<spacial::ActivityGrid> getActivityGrid() const;

protected:
	double getBlendedActivity(const double first_activity, const double second_activity,
			const BlendingMethod blending_method = BLEND_LINEAR, double force = 0.5);

private:
	Cluster & cluster;
	boost::shared_ptr<spacial::ActivityGrid> grid;
	const int DEFAULT_MESH_GRANULARITY;
	const int DEFAULT_BLEND_FORCE;
};

}

}

#endif /* MESH_H_ */
