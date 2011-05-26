/**
 * Mesh.h
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef MESH_H_
#define MESH_H_

#include "common/Cycle.h"

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

	//DEPRECATED
	/**
	 * Assignment Operator
	 *
	 * 		Overridden Assignment Operator for Mesh. Inaccessible to force singleton class
	 *
	 *  @param Mesh
	 * 		Object to Assign this to
	 *
	 Mesh & operator=(const Mesh &);
	 /**
	 * Singleton accessor for object
	 *
	 * 		Returns the single instance of a mesh, creating it if it doesn't exist yet
	 *
	 * @return boost::shared_ptr<Mesh>
	 * 		The singleton instance of the mesh
	 *
	 static boost::shared_ptr< Mesh > getMesh();
	 */

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
protected:

private:
	Cluster & cluster;

};

}

}

#endif /* MESH_H_ */
