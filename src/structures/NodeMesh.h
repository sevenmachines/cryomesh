/*
 * NodeMesh.h
 *
 *  Created on: 22 Jun 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef NODEMESH_H_
#define NODEMESH_H_

namespace cryomesh {

namespace structures {

/**
 * Mesh of nodes and their neighbouring nodes and distances
 */
class NodeMesh {
public:
	NodeMesh();
	virtual ~NodeMesh();
};

}

}

#endif /* NODEMESH_H_ */
