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
	//ss << "(";
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
	//ss << ")" << std::endl;

	// add entry to history
	std::vector<std::string> * vec_ptr = static_cast<std::vector<std::string> *> (results);
	vec_ptr->push_back(ss.str());
	//std::cout << ss.str() << std::endl;
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

void DatabaseManager::createTables() {
	sqlCommand(DatabaseManager::NODES_TABLE_FORMAT.getCreateTable());
	sqlCommand(DatabaseManager::CONNECTIONS_TABLE_FORMAT.getCreateTable());
}

void DatabaseManager::clearTables() {
	clearTable("nodesTable");
	clearTable("connectionsTable");
}

std::string DatabaseManager::clearTable(const std::string & table) {
	std::stringstream ss;
	ss << "delete from " << table;
	return this->sqlCommand(ss.str());
}

std::string DatabaseManager::insertNode(const DatabaseObject & db_object) {
	return sqlCommand(db_object.getInsert(NODES_TABLE_FORMAT.getName()));
}
std::string DatabaseManager::insertConnection(const DatabaseObject & db_object) {
	return sqlCommand(db_object.getInsert(CONNECTIONS_TABLE_FORMAT.getName()));
}

std::string DatabaseManager::selectNodes(const std::string & criteria) {
	return select("nodesTable", criteria);
}
std::string DatabaseManager::selectConnections(const std::string & criteria) {
	return select("connectionsTable", criteria);
}
std::string DatabaseManager::select(const std::string & table, const std::string & criteria) {
	return sqlCommandBySelection(table, "SELECT", criteria);
}

std::string DatabaseManager::deleteNodes(const std::string & criteria) {
	return deleteSelected("nodesTable", criteria);
}
std::string DatabaseManager::deleteConnections(const std::string & criteria) {
	return deleteSelected("connectionsTable", criteria);
}

std::string DatabaseManager::deleteSelected(const std::string & table, const std::string & criteria) {
	return sqlCommandBySelection(table, "DELETE", criteria);
}

int DatabaseManager::countNodes() {
	return (this->countRows("nodesTable"));
}
int DatabaseManager::countConnections() {

	return (this->countRows("connectionsTable"));
}
int DatabaseManager::countRows(const std::string & table) {
	std::stringstream ss;
	int number = 0;
	ss << "SELECT count(*) FROM " << table << ";";
	std::string result = sqlCommand(ss.str());
	if (result.size() > 0) {
		std::string numstr = result.substr(result.find(":") + 1, 1);
		//std::cout<<"DatabaseManager::countRows: "<<result <<" -> "<<numstr<<std::endl;
		number = atoi(numstr.c_str());
	}
	//std::cout << "DatabaseManager::countRows: " << number << std::endl;
	return number;
}

std::string DatabaseManager::sqlCommandBySelection(const std::string & table, const std::string & command,
		const std::string & criteria) {
	std::stringstream ss;
	ss << command << " * FROM " << table;
	if (criteria != "") {
		ss << " WHERE " << criteria;
	}
	ss << ";";
	return sqlCommand(ss.str());
}

std::string DatabaseManager::dropTable(const std::string & table) {
	// Drop the table from database
	std::stringstream ss;
	ss << "drop table " << table;
	return sqlCommand(ss.str());
}

std::string DatabaseManager::sqlCommand(const std::string & command) {
	sqlResultsBuffer.clear();
	bool success = false;
	std::string results;
	errorCode = sqlite3_exec(database, command.c_str(), &databaseCallback, &sqlResultsBuffer, &errorMessage);
	// do results
	{
		std::stringstream ss;
		// forall in sqlResultsBuffer
		{
			std::vector<std::string>::const_iterator it_sqlResultsBuffer = sqlResultsBuffer.begin();
			const std::vector<std::string>::const_iterator it_sqlResultsBuffer_end = sqlResultsBuffer.end();
			while (it_sqlResultsBuffer != it_sqlResultsBuffer_end) {
				ss << *it_sqlResultsBuffer;
				++it_sqlResultsBuffer;
				if (it_sqlResultsBuffer != it_sqlResultsBuffer_end) {
					ss << std::endl;
				}
			}
			results = ss.str();
		}

	}
	DatabaseManager::addHistoryEntry(command, sqlResultsBuffer, sqlResults);
	if (errorCode != SQLITE_OK) {
		std::cout << "DatabaseManager::sqlCommand: " << "ERROR: " << errorMessage << std::endl;
		std::cout << "\t" << "Command: " << command << std::endl;
		success = false;
	} else {
		success = true;
	}
	sqlite3_free(errorMessage);
	return results;
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

std::ostream & DatabaseManager::printHistory(std::ostream & os, unsigned int countback) {
	common::Cycle now = common::TimeKeeper::getTimeKeeper().getCycle();
	common::Cycle zero(0);
	common::Cycle start = std::max(zero, now - countback);
	for (common::Cycle temp_cycle = std::max(zero, now - countback); temp_cycle <= now; temp_cycle++) {
		printHistory(os, temp_cycle);
	}
	return os;
}

std::ostream & DatabaseManager::printHistory(std::ostream & os, const common::Cycle & cycle) {

	// forall in sqlResults
	{
		std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlResults =
				sqlResults.begin();
		const std::multimap<common::Cycle, std::pair<std::string, std::string> >::const_iterator it_sqlResults_end =
				sqlResults.end();
		while (it_sqlResults != it_sqlResults_end) {
			if (it_sqlResults->first == cycle) {
				//std::cout << "DatabaseManager::printHistory: " << it_sqlResults->first.toULInt() << std::endl;
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
