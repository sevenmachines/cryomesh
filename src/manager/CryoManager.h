/*
 * CryoManager.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CRYOMANAGER_H_
#define CRYOMANAGER_H_

namespace cryomesh {
namespace manager {

/**
 * Control manager for cryomesh's. Should be the one point where clients/servers etc interact
 * with the actual mesh, databases are managed, and the mesh is created and managed in general
 */
class CryoManager {
public:
	CryoManager();
	virtual ~CryoManager();
};

}//NAMESPACE
}//NAMESPACE

#endif /* CRYOMANAGER_H_ */
