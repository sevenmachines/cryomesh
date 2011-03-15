/*
 * DatabaseManager.cpp
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <fstream>

namespace cryomesh {

namespace manager {

const std::string DatabaseManager::DEFAULT_DATABASE = "default.db";
 const DatabaseManager::NodeTableFormat DatabaseManager::NODES_TABLE_FORMAT;
 const DatabaseManager::ConnectionTableFormat DatabaseManager::CONNECTIONS_TABLE_FORMAT;

int DatabaseManager::databaseCallback(void *unused, int argc, char **argv, char **columnName) {
	int i;
	std::cout << "(";
	for (i = 0; i < argc; i++) {
		std::string col_name = columnName[i];
		std::string entry;
		if (argv[i] == 0) {
			entry = "NULL";
		} else {
			entry = argv[i];
		}
		std::cout << " " << col_name << ":" << entry << " ";
	}
	std::cout << ")" << std::endl;
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
		std::cout << "DatabaseManager::createTables: " << "ERROR: " <<DatabaseManager::NODES_TABLE_FORMAT.getCreateTable()<< std::endl;
	}

	success = sqlCommand(DatabaseManager::CONNECTIONS_TABLE_FORMAT.getCreateTable());
	if (success == false) {
		std::cout << "DatabaseManager::createTables: " << "ERROR: " <<DatabaseManager::CONNECTIONS_TABLE_FORMAT.getCreateTable()<< std::endl;
	}

	return success;
}

bool DatabaseManager::clearTable(const std::string & table) {
	std::stringstream ss;
	ss << "delete from " << table;
	return this->sqlCommand(ss.str());
}

bool DatabaseManager::insert() {
	return sqlCommand("insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)");
}
bool DatabaseManager::insertNode() {
	// TODO DatabaseManager::insertNode()
	return false;
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
	std::cout << "DatabaseManager::sqlCommand: " << command << std::endl;
	errorCode = sqlite3_exec(database, command.c_str(), DatabaseManager::databaseCallback, 0, &errorMessage);
	if (errorCode != SQLITE_OK) {
		std::cout << "DatabaseManager::sqlCommand: " << "ERROR: " << errorMessage << std::endl;
		success = false;
	} else {
		success = true;
	}
	sqlite3_free(errorMessage);
	return success;;
}

}//NAMESPACE

}//NAMESPACE
