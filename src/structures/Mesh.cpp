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

Mesh::Mesh(Cluster & clus) : cluster(clus){
	// make us the same side as the max bounding box

}

Mesh::~Mesh() {
}

void Mesh::update(){
	std::cout<<"Mesh::update: "<<"TODO"<<std::endl;
}

Node & Mesh::warp( Node & node) {
	//TODO Mesh::warp (const Node&)
	std::cout<<"Mesh::warp(Node &): "<<"TODO"<<std::endl;
	return node;
}

ImpulseCollection & Mesh::warp(ImpulseCollection & impulseCollection){
	//TODO Mesh::warp (ImpulseCollection&)
	std::cout<<"Mesh::warp(ImpulseCollection &): "<<"TODO"<<std::endl;
	return impulseCollection;
}

Impulse & Mesh::warp(Impulse & impulse){
	//TODO Mesh::warp (Impulse&)
	std::cout<<"Mesh::warp(Impulse &): "<<"TODO"<<std::endl;
	return impulse;
}

}//NAMESPACE

}//NAMESPACE
