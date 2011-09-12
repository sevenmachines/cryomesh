/*
 * DatabaseManager.h
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef DATABASEMANAGER_H_
#define DATABASEMANAGER_H_

#include "TableFormats.h"
#include "NodeDatabaseObject.h"
#include "ConnectionDatabaseObject.h"
#include "common/Cycle.h"
#include "common/TimeKeeper.h"

#include <sqlite3.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <map>
#include <sstream>

namespace cryomesh {

namespace manager {

/**
 * Database manager creates and maintains a database of mesh related objects and data
 */
class DatabaseManager {
public:

	/**
	 * Default constructor using a filename as the database or a default
	 * Opens the file or creates it and creates the tables if it doesnt exist
	 *
	 * @param std::string
	 * 	The name of the database to open/create
	 */
	DatabaseManager(const std::string & dbfile = DEFAULT_DATABASE);
	DatabaseManager(const DatabaseManager & obj) ;

	/**
	 * Assignment operator
	 *
	 * @param const DatabaseManager & obj
	 * 		RHS assignment
	 *
	 * @return DatabaseManager &
	 * 		This object after assignment
	 */
	DatabaseManager & operator=(const DatabaseManager & obj);

	/**
	 * Default destructor closes the database
	 */
	virtual ~DatabaseManager();

	/**
	 * Check if the database is accessible
	 *
	 * @return bool
	 * 	True if deemed accessible, false otherwise
	 */
	bool isDatabaseAccessable() const;

	/**
	 * Create all needed tables
	 */
	void createTables();

	/**
	 * Clear all tables
	 */
	void clearTables();

	/**
	 * Clear all values in a table
	 *
	 * @param std::string
	 * 	The table to clear
	 *
	 * @return std::string
	 * 	The result of the sql query
	 */
	std::string clearTable(const std::string & table);

	/**
	 * Drop a table from database
	 *
	 * @param std::string
	 * 	The table to drop
	 *
	 * @return std::string
	 * 	The result of the sql query
	 */
	std::string dropTable(const std::string & table);

	/**
	 * Insert a node data object into the table
	 *
	 * @param DatabaseObject
	 * 	Database object to insert as a node
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string insertNode(const DatabaseObject & db_object);

	/**
	 * Insert a connection data object into the table
	 *
	 * @param DatabaseObject
	 * 	Database object to insert as a node
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string insertConnection(const DatabaseObject & db_object);

	std::string insertOutputPattern(const DatabaseObject & db_object);

	/**
	 * Select unique node  entry
	 *
	 * @param std::string
	 * 	The uuid of the node
	 * @param Cycle
	 * 	The cycle to select on, to force uniqueness
	 *
	 * @return std::string
	 * 	The value of the entry
	 */
	std::string selectNode(const std::string &uuid, const common::Cycle & cycle);

	/**
	 * Select unique column  entry
	 *
	 * @param std::string
	 * 	The uuid of the node
	 * @param Cycle
	 * 	The cycle to select on, to force uniqueness
	 *
	 * @return std::string
	 * 	The value of the entry
	 */
	std::string selectConnection(const std::string &uuid, const common::Cycle & cycle);

	std::string selectOutputPattern(const std::string &uuid, const common::Cycle & cycle);

	/**
	 * Select unique node column entry
	 *
	 * @param std::string
	 * 	The uuid of the node
	 * @param Cycle
	 * 	The cycle to select on, to force uniqueness
	 * @param std::string
	 * 	The column to select
	 *
	 * @return std::string
	 * 	The value of the entry
	 */
	std::string selectNodeValue(const std::string &uuid, const common::Cycle & cycle, const std::string & column);

	/**
	 * Select unique column column entry
	 *
	 * @param std::string
	 * 	The uuid of the node
	 * @param Cycle
	 * 	The cycle to select on, to force uniqueness
	 * @param std::string
	 * 	The column to select
	 *
	 * @return std::string
	 * 	The value of the entry
	 */
	std::string selectConnectionValue(const std::string &uuid, const common::Cycle & cycle, const std::string & column);

	std::string selectOutputPatternValue(const std::string &uuid, const common::Cycle & cycle,
			const std::string & column);

	/**
	 * Select unique column column entry from table
	 *
	 *	@param std::string
	 *		The table to utilise
	 * @param std::string
	 * 	The uuid of the node
	 * @param Cycle
	 * 	The cycle to select on, to force uniqueness
	 * @param std::string
	 * 	The column to select
	 *
	 * @return std::string
	 * 	The value of the entry
	 */
	std::string selectValue(const std::string & table, const std::string &uuid, const common::Cycle & cycle,
			const std::string & column);

	/**
	 * Select nodes by a criteria string eg, 'id=erwrs324 AND cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string selectNodes(const std::string & criteria = "");

	/**
	 * Select connections by a criteria string eg, 'id=erwrs324 AND cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string selectConnections(const std::string & criteria = "");

	std::string selectOutputPatterns(const std::string & criteria = "");

	/**
	 * Delete node by uuid
	 *
	 * @param std::string
	 * 	The uuid to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string deleteNode(const std::string & id);

	/**
	 * Delete nodes by a criteria string eg, 'id=erwrs324 AND cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string deleteNodes(const std::string & criteria = "");

	/**
	 * Delete connection by uuid
	 *
	 * @param std::string
	 * 	The uuid to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string deleteConnection(const std::string & id);

	/**
	 * Delete nodes by a criteria string eg, 'id=erwrs324 AND cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string deleteConnections(const std::string & criteria = "");

	std::string deleteOutputPattern(const std::string & id);
	std::string deleteOutputPatterns(const std::string & criteria = "");

	/**
	 * Delete objects from a table by a criteria string eg, 'id=erwrs324 AND cycle=1'
	 *
	 * @param std::string
	 * 	The table to delete from
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return std::string
	 * 	Result of sql query
	 */
	std::string deleteSelected(const std::string & table, const std::string & criteria = "");

	/**
	 * Count nodes  by a criteria string eg, 'cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return int
	 * 	The result of the count
	 */
	int countNodes(const std::string & criteria = "");

	/**
	 * Count connections  by a criteria string eg, 'cycle=1'
	 *
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return int
	 * 	The result of the count
	 */
	int countConnections(const std::string & criteria = "");

	/**
	 * Count objects from a table by a criteria string eg, 'cycle=1'
	 *
	 * @param std::string
	 * 	The table to count from
	 * @param std::string
	 * 	The criteria to match
	 *
	 *@return int
	 * 	The result of the count
	 */
	int countRows(const std::string & table, const std::string & criteria = "");

	/**
	 * update node from using options list
	 *
	 * @param std::string
	 * 	The id to match
	 * @param common::Cycle
	 * 	The cycle to match
	 * @param std::string
	 * 	The options to set
	 *
	 *	@return int
	 * 	The result of the count
	 */
	std::string updateNode(const std::string & uuid_str, const common::Cycle & cycle, const std::string & options);

	/**
	 * update node from using options list
	 *
	 * @param std::string
	 * 	The id to match
	 * @param common::Cycle
	 * 	The cycle to match
	 * @param std::string
	 * 	The options to set
	 *
	 *	@return int
	 * 	The result of the count
	 */
	std::string
	updateConnection(const std::string & uuid_str, const common::Cycle & cycle, const std::string & options);

	/**
	 * update object from a table using options list
	 *
	 * @param std::string
	 * 	The id to match
	 * @param common::Cycle
	 * 	The cycle to match
	 * @param std::string
	 * 	The options to set
	 * @param std::string
	 * 	The table to use
	 *
	 *	@return int
	 * 	The result of the count
	 */
	std::string updateByUUID(const std::string & uuid_str, const common::Cycle & cycle, const std::string & options,
			const std::string & table);
	/**
	 * Select all columns from table using criteria
	 *
	 * @param std::string
	 * 	Name of table
	 *	@param std::string
	 *		Selection criteria
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string select(const std::string & table, const std::string & criteria = "");

	/**
	 * delete all data from table
	 *
	 * @param std::string
	 * 	Name of table
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string deleteAll(const std::string & table);

	/**
	 * delete all by cycle
	 *
	 * @param common::Cycle
	 * 	Cycle to compare against
	 * @param int
	 * 	The type of comparison to make, <0 for less than, ==0 for equals, and >0 for greater than
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string deleteAllByCycle(const common::Cycle & cycle, int comparison_type);

	/**
	 * delete nodes by cycle
	 *
	 * @param common::Cycle
	 * 	Cycle to compare against
	 * @param int
	 * 	The type of comparison to make, <0 for less than, ==0 for equals, and >0 for greater than
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string deleteNodesByCycle(const common::Cycle & cycle, int comparison_type);

	/**
	 * delete connections by cycle
	 *
	 * @param common::Cycle
	 * 	Cycle to compare against
	 * @param int
	 * 	The type of comparison to make, <0 for less than, ==0 for equals, and >0 for greater than
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string deleteConnectionsByCycle(const common::Cycle & cycle, int comparison_type);

	/**
	 * delete objects from table  by cycle
	 *
	 *	@param std::string
	 *		The table to delete from
	 * @param common::Cycle
	 * 	Cycle to compare against
	 * @param int
	 * 	The type of comparison to make, <0 for less than, ==0 for equals, and >0 for greater than
	 *
	 *	@return std::string
	 *		sql query results
	 */
	std::string deleteByCycle(const std::string & table, const common::Cycle & cycle, int comparison_type);

	/**
	 * Print sql history to output stream
	 *
	 * @param std::ostream
	 * 	Output stream to print to
	 *@param Cycle
	 *		The cycle to print information on
	 *
	 * @return std::ostream
	 * 	Return the supplied output stream
	 */
	std::ostream & printHistory(std::ostream & os, const common::Cycle & cycle);

	/**
	 * Print sql history to output stream
	 *
	 * @param std::ostream
	 * 	Output stream to print to
	 *@param unsigned int
	 *		Muber of cycles of previous history to print
	 *
	 * @return std::ostream
	 * 	Return the supplied output stream
	 */
	std::ostream & printHistory(std::ostream & os, unsigned int countback = 1);

	/**
	 * Default database file
	 *
	 *	@var std::string
	 */
	static const std::string DEFAULT_DATABASE;
	static const std::string DEFAULT_DATABASE_PATH;

	/**
	 * Function that is called on finishing an sql command
	 */
	static int databaseCallback(void *unused, int argc, char **argv, char **columnName);

	/**
	 * Default node table format
	 *
	 * @var NodeTableFormat
	 */
	static const NodeTableFormat NODES_TABLE_FORMAT;

	/**
	 * Default connection table format
	 */
	static const ConnectionTableFormat CONNECTIONS_TABLE_FORMAT;

	static const OutputPatternsTableFormat OUTPUT_PATTERNS_TABLE_FORMAT;

protected:
	/**
	 * Database
	 *
	 *	@var sqlite *
	 */
	sqlite3 * database;

	/**
	 * Record error codes
	 *
	 * @var int
	 */
	int errorCode;

	/**
	 * Record error message
	 *
	 * @var char *
	 */
	char * errorMessage;

	/**
	 * Database accessable
	 *
	 * @bool
	 */
	bool databaseAccess;

	/**
	 * History of sql results
	 *
	 * @var std::map<common::Cycle, std::string>
	 */
	std::multimap<common::Cycle, std::pair<std::string, std::string> > sqlResults;

	/**
	 * Buffer container for sql results
	 *
	 * @var std::vector<std::string>
	 */
	std::vector<std::string> sqlResultsBuffer;

	/**
	 * Maximum number of historical cycle log to keep
	 *
	 * @var common::Cycle
	 */
	static const common::Cycle MAX_COMMAND_HISTORY;

	/**
	 * Run a provided sql command string
	 *
	 * @param std::string
	 *		The command string to run
	 *
	 *	@return std::vector<std::string>
	 *		vector of results
	 */
	std::string sqlCommand(const std::string & command);

	std::string sqlCommandBySelection(const std::string & table, const std::string & command,
			const std::string & criteria);

	/**
	 * Add an entry to a historical multimap
	 *
	 * @param std::string
	 * 	Entry to add
	 * @param std::multimap<std::string, std::string>
	 * 	Map to add entry to
	 *
	 * @return std::multimap<std::string, std::string>
	 * 	Return the modified map
	 */
	static const std::multimap<common::Cycle, std::pair<std::string, std::string> > & addHistoryEntry(
			const std::string & command, const std::string & results,
			std::multimap<common::Cycle, std::pair<std::string, std::string> > & map);

	/**
	 * Add an list of entries to a historical multimap
	 *
	 * @param std::vector<std::string>
	 * 	Entries to add
	 * @param std::multimap<std::string, std::string>
	 * 	Map to add entry to
	 *
	 * @return std::multimap<std::string, std::string>
	 * 	Return the modified map
	 */
	static const std::multimap<common::Cycle, std::pair<std::string, std::string> > & addHistoryEntry(
			const std::string & command, const std::vector<std::string> & results,
			std::multimap<common::Cycle, std::pair<std::string, std::string> > & map);
};

}

}

#endif /* DATABASEMANAGER_H_ */
