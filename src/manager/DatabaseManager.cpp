/*
 * DatabaseManager.cpp
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "DatabaseManager.h"
#include "common/TimeKeeper.h"
#include "common/Containers.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace cryomesh {

namespace manager {

const std::string DatabaseManager::DEFAULT_DATABASE = "default.db";
const DatabaseManager::NodeTableFormat DatabaseManager::NODES_TABLE_FORMAT;
const DatabaseManager::ConnectionTableFormat DatabaseManager::CONNECTIONS_TABLE_FORMAT;
const common::Cycle DatabaseManager::MAX_COMMAND_HISTORY = common::Cycle(100);

int DatabaseManager::databaseCallback(void *results, int argc, char **argv, char **columnName) {
	int i;
	std::stringstream ss;
	ss << "(";
	for (i = 0; i < argc; i++) {
		std::string col_name = columnName[i];
		std::string entry;
		if (argv[i] == 0) {
			entry = "NULL";
		} else {
			entry = argv[i];
		}
		ss << " " << col_name << ":" << entry << " ";
	}
	ss << ")" << std::endl;

	// add entry to history
	std::vector<std::string> * vec_ptr = static_cast<std::vector<std::string> *> (results);
	vec_ptr->push_back(ss.str());
	return 0;
}

DatabaseManager::DatabaseManager(const std::string & dbfile) :
	errorCode(-1), errorMessage(0), databaseAccess(false) {

	//try open already existing
	char * null_ptr = 0;
	errorCode = sqlite3_open_v2(dbfile.c_str(), &database, SQLITE_OPEN_READWRITE, null_ptr);
	if (errorCode != 0) {
		// open failed to force creation
		errorCode = sqlite3_open(dbfile.c_str(), &database);
		if (errorCode != 0) {
			sqlite3_close(database);
			std::cout << "DatabaseManager::DatabaseManager: " << "ERROR creating new database " << dbfile << std::endl;
			std::cout << sqlite3_errmsg(database) << std::endl;
			databaseAccess = false;
		} else {
			std::cout << "DatabaseManager::DatabaseManager: " << "Created new database " << dbfile << std::endl;
			databaseAccess = true;
			createTables();
		}
	} else {
		std::cout << "DatabaseManager::DatabaseManager: " << "Opened existing database " << dbfile << std::endl;
		databaseAccess = true;
	}
}

DatabaseManager::~DatabaseManager() {
	sqlite3_close(database);
	databaseAccess = false;
}

bool DatabaseManager::isDatabaseAccessable() const {
	return databaseAccess;
}

bool DatabaseManager::createTables() {
	bool success = true;

	success = sqlCommand(DatabaseManager::NODES_TABLE_FORMAT.getCreateTable());
	if (success == false) {
		std::cout << "DatabaseManager::createTables: " << "ERROR: "
				<< DatabaseManager::NODES_TABLE_FORMAT.getCreateTable() << std::endl;
	}

	success = sqlCommand(DatabaseManager::CONNECTIONS_TABLE_FORMAT.getCreateTable());
	if (success == false) {
		std::cout << "DatabaseManager::createTables: " << "ERROR: "
				<< DatabaseManager::CONNECTIONS_TABLE_FORMAT.getCreateTable() << std::endl;
	}

	return success;
}

bool DatabaseManager::clearTable(const std::string & table) {
	std::stringstream ss;
	ss << "delete from " << table;
	return this->sqlCommand(ss.str());
}

bool DatabaseManager::insertNode(const DatabaseObject & db_object) {
	return sqlCommand(db_object.getInsert(NODES_TABLE_FORMAT.getName()));
}
bool DatabaseManager::insertConnection() {
	// TODO DatabaseManager::insertConnection()
	return false;
}
bool DatabaseManager::selectNode() {
	// TODO DatabaseManager::selectNode()
	return false;
}
bool selectConnection() {
	// TODO DatabaseManager::selectConnection()
	return false;
}
bool DatabaseManager::DatabaseManager::selectAll() {
	// Select all data in myTable
	return sqlCommand("select * from nodesTable");
}

bool DatabaseManager::deleteAll() {
	// Remove all data in myTable
	return sqlCommand("delete from nodesTable");
}

bool DatabaseManager::dropTable(const std::string & table) {
	// Drop the table from database
	std::stringstream ss;
	ss << "drop table " << table;
	return sqlCommand(ss.str());
}

bool DatabaseManager::sqlCommand(const std::string & command) {
	bool success = false;
	errorCode = sqlite3_exec(database, command.c_str(), &databaseCallback, &sqlResultsBuffer, &errorMessage);
	// do results
	DatabaseManager::addHistoryEntry(command, sqlResultsBuffer, sqlResults);
	sqlResultsBuffer.clear();
	if (errorCode != SQLITE_OK) {
		std::cout << "DatabaseManager::sqlCommand: " << "ERROR: " << errorMessage << std::endl;
		success = false;
	} else {
		success = true;
	}
	sqlite3_free(errorMessage);
	return success;
}

const std::multimap<common::Cycle, std::pair<std::string, std::string> > & DatabaseManager::addHistoryEntry(
		const std::string & command, const std::vector<std::string> & results,
		std::multimap<common::Cycle, std::pair<std::string, std::string> > & map) {
	std::stringstream ss;
	// forall in entries
	{
		std::vector<std::string>::const_iterator it_entries = results.begin();
		const std::vector<std::string>::const_iterator it_entries_end = results.end();
		while (it_entries != it_entries_end) {
			ss << "\t" << *it_entries;
			++it_entries;
			if (it_entries != it_entries_end) {
				std::cout << std::endl;
			}
		}
	}
	addHistoryEntry(command, ss.str(), map);
	return map;
}
const std::multimap<common::Cycle, std::pair<std::string, std::string> > & DatabaseManager::addHistoryEntry(
		const std::string & command, const std::string & results,
		std::multimap<common::Cycle, std::pair<std::string, std::string> > & map) {
	common::Cycle now = common::TimeKeeper::getTimeKeeper().getCycle();
	common::Cycle cuttoff = now - MAX_COMMAND_HISTORY;

	std::pair<std::string, std::string> new_entry_pair(command, results);
	std::pair<common::Cycle, std::pair<std::string, std::string> > new_pair(now, new_entry_pair);
	map.insert(new_pair);
	common::Containers::deleteByComparison(cuttoff, map, -1);
	return map;
}

std::ostream & DatabaseManager::printHistory(std::ostream & os, const common::Cycle & cycle) {

	// forall in sqlResults
	{
		std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlResults =
				sqlResults.begin();
		const std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlResults_end =
				sqlResults.end();
		while (it_sqlResults != it_sqlResults_end) {
			if (it_sqlResults->first.toULInt() == cycle.toULInt()) {
				std::cout<<"DatabaseManager::printHistory: "<<it_sqlResults->first.toULInt() <<std::endl;
				os << "Cycle:" << it_sqlResults->first << std::endl;
				os << "Command: " << it_sqlResults->second.first << std::endl;
				os << "Result: " << it_sqlResults->second.second << std::endl;
			}
			++it_sqlResults;
		}
	}

	/*
	std::cout << "DatabaseManager::printHistory: " << sqlResults.size() << std::endl;
	std::stringstream ss;
	common::Cycle now = common::TimeKeeper::getTimeKeeper().getCycle();
	const std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlresults_end =
			sqlResults.end();

	long int cutoff_int = std::max(0l, now.toLInt() - MAX_COMMAND_HISTORY.toLInt());

	std::cout << "DatabaseManager::printHistory: " << "range (" << cutoff_int << ":" << now.toULInt() << ")"
			<< std::endl;
	for (long int i = cutoff_int; i < now.toLInt(); i++) {
		std::cout << "DatabaseManager::printHistory: " << "i: " << i << std::endl;
		std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlresults_found =
				sqlResults.find(common::Cycle(i));
		std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlresults =
				it_sqlresults_found;
		ss << "DatabaseManager: " << "cycle: " << i << std::endl;
		if (it_sqlresults != it_sqlresults_end) {//&& it_sqlresults->first == common::Cycle(i)) {
			std::cout << "DatabaseManager::printHistory: " << "FOUND" << std::endl;
			ss << "$ " << it_sqlresults->second.first << std::endl;
			ss << it_sqlresults->second.second << std::endl;
		}
	}
	os << ss.str() << std::endl;
*/
	return os;
}
}//NAMESPACE

}//NAMESPACE
