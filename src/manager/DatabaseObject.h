/*
 * DatabaseObject.h
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef DATABASEOBJECT_H_
#define DATABASEOBJECT_H_

#include <map>
#include <string>
#include <sstream>

namespace cryomesh {

namespace manager {

class DatabaseObject {
public:
	DatabaseObject() {
	}
	virtual ~DatabaseObject() {
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
	std::string getKey(const std::string & key) const{
		std::string obj = "";
		std::map<std::string, std::string>::const_iterator it_found = columns.find(key);
		if (it_found != columns.end()) {
			obj = it_found->second;
		}
		return obj;
	}

	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const =0;

	template<class T>
	static std::string toString(T obj) {
		std::stringstream ss;
		ss << obj;
		return ss.str();
	}
protected:
	/**
	 * key/object pair map of column names to type
	 *
	 * @var std::map<std::string, std::string>
	 */
	std::map<std::string, std::string> columns;
};

}

}

#endif /* DATABASEOBJECT_H_ */
