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
		PAUSED, RUNNING, STOPPED, EMPTY, NEW
	};

	/**
	 * Default contructor
	 */
	CryoManager() :
		currentState(EMPTY) {
	}

	/**
	 * Default destructor
	 */
	virtual ~CryoManager() {
	}

	/**
	 * Set manager state to create, load config from file
	 *
	 * @param std::string
	 * 	The config file to create from
	 */
	void create(const std::string configfile) {
		std::cout << "CryoManager::create: " << "" << std::endl;
		currentState = NEW;
		creator = boost::shared_ptr<Creator>(new Creator(configfile));
	}

	/**
	 * Set manager state to running
	 */
	void run() {
		std::cout << "CryoManager::run: " << "" << std::endl;
		currentState = RUNNING;
	}

	/**
	 * Set manager state to paused
	 */
	void pause() {
		std::cout << "CryoManager::pause: " << "" << std::endl;
		currentState = PAUSED;
	}

	/**
	 * Set manager state to stopped
	 */
	void stop() {
		std::cout << "CryoManager::stop: " << "" << std::endl;
		currentState = STOPPED;
	}

	/**
	 * Set manager state to destroy
	 */
	void destroy() {
		std::cout << "CryoManager::destroy: " << "" << std::endl;
		currentState = EMPTY;
		creator.reset();
	}

	/**
	 * Return the managers current state
	 *
	 * @return ManagerState
	 * 	The current state of the manager
	 */
	const ManagerState & getCurrentState() const {
		return currentState;
	}

private:
	/**
	 * The current state of the manager
	 *
	 * @var ManagerState
	 */
	ManagerState currentState;

	/**
	 * The creator for this manager
	 *
	 * @var boost::shared_ptr< Creator >
	 */
	boost::shared_ptr<Creator> creator;
};

}//NAMESPACE
}//NAMESPACE

#endif /* CRYOMANAGER_H_ */
