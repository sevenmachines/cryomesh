/*
 * CryoManager.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CRYOMANAGER_H_
#define CRYOMANAGER_H_

namespace cryomesh {

/**
 * Control manager for cryomesh's. Should be the one point where clients/servers etc interact
 * with the actual mesh
 */
class CryoManager {
public:
	CryoManager();
	virtual ~CryoManager();
};

}

#endif /* CRYOMANAGER_H_ */
