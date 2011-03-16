/*
 * DatabaseManager.h
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef DATABASEMANAGER_H_
#define DATABASEMANAGER_H_

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
	 * General structure of a table
	 */
	struct TableFormat {
	public:
		/**
		 * Return the name of the table
		 *
		 * @return std::string
		 * 	The name of the table
		 */
		std::string getName() const {
			return name;
		}

		/**
		 * Return the string object associated with a key
		 *
		 * @std::string
		 * 	The key to search for
		 *
		 *	@return std::string
		 * 	The object associated with the search key, "" if not found
		 */
		std::string getKey(const std::string & key) {
			std::string obj = "";
			std::map<std::string, std::string>::const_iterator it_found = columns.find(key);
			if (it_found != columns.end()) {
				obj = it_found->second;
			}
			return obj;
		}

		/**
		 * Get the string that can be used to create the sql table
		 *
		 * @return
		 * 	the sql command string to create this table
		 */
		std::string getCreateTable() const {
			std::stringstream ss;
			ss << "create table " << getName() << " (";
			// forall in columns
			{
				std::map<std::string, std::string>::const_iterator it_columns = columns.begin();
				const std::map<std::string, std::string>::const_iterator it_columns_end = columns.end();
				while (it_columns != it_columns_end) {
					ss << it_columns->first << " " << it_columns->second;
					++it_columns;
					if (it_columns != it_columns_end) {
						ss << ", ";
					}
				}
				ss << ");";
			}
			return ss.str();

		}

	protected:
		/**
		 * The name of the table
		 *
		 * @var std::string
		 */
		std::string name;

		/**
		 * key/object pair map of column names to type
		 *
		 * @var std::map<std::string, std::string>
		 */
		std::map<std::string, std::string> columns;
	};

	/**
	 * Struct representing a node table structure
	 */
	struct NodeTableFormat: public TableFormat {
	public:
		/**
		 * Defualt constructor will construct all the names and columns assiciated with a node table
		 */
		NodeTableFormat() {
			name = "nodesTable";
			columns["id"] = "TEXT";
			columns["x"] = "DOUBLE";
			columns["y"] = "DOUBLE";
			columns["z"] = "DOUBLE";
			columns["cycle"] = "INTEGER";
			columns["activity"] = "DOUBLE";
		}
	};

	/**
	 * Struct representing a connections table structure
	 */
	struct ConnectionTableFormat: public TableFormat {
	public:
		/**
		 * Defualt constructor will construct all the names and columns assiciated with a connections table
		 */
		ConnectionTableFormat() {
			name = "connectionsTable";
			columns["id"] = "TEXT";
			columns["inputid"] = "TEXT";
			columns["outputid"] = "TEXT";
			columns["cycle"] = "INTEGER";
			columns["impulses"] = "INTEGER";
		}
	};

	/**
	 * Default constructor using a filename as the database or a default
	 * Opens the file or creates it and creates the tables if it doesnt exist
	 */
	DatabaseManager(const std::string & dbfile = DEFAULT_DATABASE);

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
	bool isDatabaseAccessable() const ;

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
	 */
	std::string clearTable(const std::string & table);

	std::string dropTable(const std::string & table);

	/**
	 * Insert a node data object into the table
	 *
	 */
	std::string insertNode(const DatabaseObject & db_object);
	std::string insertConnection(const DatabaseObject & db_object);
	std::string selectNodes(const std::string & criteria = "");
	std::string selectConnections(const std::string & criteria = "");
	std::string deleteNodes(const std::string & criteria = "");
	std::string deleteConnections(const std::string & criteria = "");
	std::string deleteSelected(const std::string & table, const std::string & criteria);
	int countNodes() ;
	int countConnections() ;
	int countRows(const std::string & table) ;
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
	std::ostream & printHistory(std::ostream & os, unsigned int countback =1);

	/**
	 * Default database file
	 *
	 *	@var std::string
	 */
	static const std::string DEFAULT_DATABASE;

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
