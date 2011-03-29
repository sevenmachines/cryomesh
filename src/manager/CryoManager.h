/*
 * CryoManager.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CRYOMANAGER_H_
#define CRYOMANAGER_H_

#include <iostream>

namespace cryomesh {
namespace manager {

/**
 * Control manager for cryomesh's. Should be the one point where clients/servers etc interact
 * with the actual mesh, databases are managed, and the mesh is created and managed in general
 */
class CryoManager {
public:
	/**
	 * Enum to specify the actual state of the manager/cryomesh
	 */
	enum ManagerState {
		PAUSED, RUNNING, STOPPED,  EMPTY, NEW
	};
	CryoManager();
	virtual ~CryoManager();
	void create(){
		currentState = NEW;
		std::cout<<"CryoManager::create: "<<""<<std::endl;
	}
	void run(){
		currentState = RUNNING;
		std::cout<<"CryoManager::run: "<<""<<std::endl;
	}
	void pause(){
		currentState = PAUSED;
		std::cout<<"CryoManager::pause: "<<""<<std::endl;
	}
	void stop(){
		currentState = STOPPED;
		std::cout<<"CryoManager::stop: "<<""<<std::endl;
	}
	void destroy(){
		currentState = EMPTY;
		std::cout<<"CryoManager::destroy: "<<""<<std::endl;
	}

	const ManagerState & getCurrentState()const{
		return currentState;
	}

private:
	ManagerState currentState;
};

}//NAMESPACE
}//NAMESPACE

#endif /* CRYOMANAGER_H_ */
