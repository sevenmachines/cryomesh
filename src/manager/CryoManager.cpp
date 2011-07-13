/*
 * CryoManager.cpp
 *
 *  Created on: 13 Jul 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */
#include "CryoManager.h"

namespace cryomesh {
namespace manager {

	CryoManager::CryoManager() :
		currentState(EMPTY) {
		srand( time(NULL));
		time_t rawtime;
		time(&rawtime);
		tm * ptm = gmtime(&rawtime);
		std::stringstream dbname;
		dbname << "cryodatabase-" << ptm->tm_yday<<ptm->tm_yday<<ptm->tm_hour<< ptm->tm_min<<ptm->tm_sec<< ".db";
		database = boost::shared_ptr<DatabaseManager>(new DatabaseManager(dbname.str()));
	}

	CryoManager::~CryoManager() {
	}

	void CryoManager::create(const std::string configfile) {
		std::cout << "CryoManager::create: " << "" << std::endl;
		currentState = NEW;
		creator = boost::shared_ptr<manager::Creator>(new manager::Creator(configfile));
	}

	void CryoManager::run() {
		currentState = RUNNING;
	}

	void CryoManager::runCycle() {
		std::cout << "CryoManager::runCycle: " << common::TimeKeeper::getTimeKeeper().getCycle() << std::endl;
		boost::shared_ptr<structures::Bundle> bundle = creator->getBundle();
		if (bundle != 0) {
			currentState = RUNNING;
			bundle->update();
			const std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> > current_patterns =
					creator->getBundle()->getActualOutputChannelsMap().getPatterns();
			this->storeCurrentActualOutputPatterns(current_patterns);
			this->stop();
		} else {
			std::cout << "CryoManager::runCycle: " << "ERROR: Bundle is null..." << std::endl;
			assert(false);
		}

	}

	void CryoManager::pause() {
		std::cout << "CryoManager::pause: " << "" << std::endl;
		currentState = PAUSED;
	}

	void CryoManager::stop() {
		std::cout << "CryoManager::stop: " << "" << std::endl;
		currentState = STOPPED;
	}

	void CryoManager::destroy() {
		std::cout << "CryoManager::destroy: " << "" << std::endl;
		currentState = EMPTY;
		creator.reset();
	}

	const CryoManager::ManagerState & CryoManager::getCurrentState() const {
		return currentState;
	}

	const boost::shared_ptr<structures::Bundle> CryoManager::getBundle() const {
		return creator->getBundle();
	}
	boost::shared_ptr<structures::Bundle> CryoManager::getMutableBundle() {
		return creator->getMutableBundle();
	}

	void CryoManager::storeCurrentActualOutputPatterns(
			const std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> > & current_patterns) {

		const common::Cycle now_cycle = common::TimeKeeper::getTimeKeeper().getCycle();
		// forall in current_patterns
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> >::const_iterator it_current_patterns =
					current_patterns.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> >::const_iterator
					it_current_patterns_end = current_patterns.end();
			while (it_current_patterns != it_current_patterns_end) {
				state::Pattern & temp_pat = *(it_current_patterns->second);
				std::stringstream temp_ss;
				temp_ss << it_current_patterns->first;
				PatternDatabaseObject po(temp_ss.str(), now_cycle, temp_pat);
				if (database != 0) {
					std::string output = database->insertOutputPattern(po);
				} else {
					std::cout << "storeCurrentActualOutputPatterns: " << "WARNING: Null database, not updating"
							<< std::endl;
				}
				++it_current_patterns;
			}
		}
	}

}//NAMESPACE
}//NAMESPACE
