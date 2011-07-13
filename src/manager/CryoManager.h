/*
 * CryoManager.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#define CRYOMANAGER_DEBUG

#ifndef CRYOMANAGER_H_
#define CRYOMANAGER_H_

#include <iostream>
#include <ctime>
#include "Creator.h"
#include "common/TimeKeeper.h"
#include "PatternDatabaseObject.h"
#include "DatabaseManager.h"
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

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
	CryoManager() ;

	/**
	 * Default destructor
	 */
	virtual ~CryoManager();

	/**
	 * Set manager state to create, load config from file
	 *
	 * @param std::string
	 * 	The config file to create from
	 */
	void create(const std::string configfile) ;

	/**
	 * Set manager state to running
	 */
	void run() ;

	/**
	 * Set manager state to run one cycle
	 */
	void runCycle() ;

	/**
	 * Set manager state to paused
	 */
	void pause() ;

	/**
	 * Set manager state to stopped
	 */
	void stop();

	/**
	 * Set manager state to destroy
	 */
	void destroy() ;

	/**
	 * Return the managers current state
	 *
	 * @return ManagerState
	 * 	The current state of the manager
	 */
	const ManagerState & getCurrentState() const ;

	//TODO Temporary access, replace with database calls instead
	const boost::shared_ptr<structures::Bundle> getBundle() const ;

	boost::shared_ptr<structures::Bundle> getMutableBundle() ;

protected:
	void storeCurrentActualOutputPatterns(
			const std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> > & current_patterns) ;

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

	boost::shared_ptr<DatabaseManager> database;
};

}//NAMESPACE
}//NAMESPACE

#endif /* CRYOMANAGER_H_ */
