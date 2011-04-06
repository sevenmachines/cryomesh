/*
 * TableFormats.h
 *
 *  Created on: 16 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef TABLEFORMATS_H_
#define TABLEFORMATS_H_

#include <string>
#include <map>
#include <sstream>

namespace cryomesh {

namespace manager {

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
	 * Default constructor will construct all the names and columns assiciated with a node table
	 */
	NodeTableFormat() {
		name = "nodesTable";
		columns["id"] = "TEXT		NOT NULL";
		columns["x"] = "DOUBLE";
		columns["y"] = "DOUBLE";
		columns["z"] = "DOUBLE";
		columns["cycle"] = "INTEGER	NOT NULL";
		columns["activity"] = "DOUBLE";
	}
};

/**
 * Struct representing a connections table structure
 */
struct ConnectionTableFormat: public TableFormat {
public:
	/**
	 * Default constructor will construct all the names and columns assiciated with a connections table
	 */
	ConnectionTableFormat() {
		name = "connectionsTable";
		columns["id"] = "TEXT		NOT NULL";
		columns["inputid"] = "TEXT";
		columns["outputid"] = "TEXT";
		columns["cycle"] = "INTEGER	NOT NULL";
		columns["impulses"] = "INTEGER";
	}
};

/**
 * Struct representing input pattern table structure
 */
struct InputPatternsTableFormat: public TableFormat {
public:
	/**
	 * Default constructor will construct all the names and columns assiciated with a pattern table
	 */
	InputPatternsTableFormat() {
		name = "inputPatternsTable";
		columns["id"] = "TEXT		NOT NULL";
		columns["cycle"] = "INTEGER	NOT NULL";
		columns["pattern"] = "TEXT";
	}
};

/**
 * Struct representing output pattern table structure
 */
struct OutputPatternsTableFormat: public TableFormat {
public:
	/**
	 * Default constructor will construct all the names and columns assiciated with a pattern table
	 */
	OutputPatternsTableFormat() {
		name = "outputPatternsTable";
		columns["id"] = "TEXT		NOT NULL";
		columns["cycle"] = "INTEGER	NOT NULL";
		columns["pattern"] = "TEXT";
	}
};

}//NAMESPACE

}//NAMESPACE

#endif /* TABLEFORMATS_H_ */
