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
#include <iostream>
#include <boost/tokenizer.hpp>

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
	std::string getKey(const std::string & key) const {
		std::string obj = "";
		std::map<std::string, std::string>::const_iterator it_found = columns.find(key);
		if (it_found != columns.end()) {
			obj = it_found->second;
		}
		return obj;
	}


	/**
	 * Find entries value in map or return null
	 *
	 * @param std::string
	 * 	Entry to find
	 * @param std::map<std::string, std::string
	 * 	map to search
	 *
	 * @return
	 * 	Value of entry
	 */

	static std::string findValue(const std::string & entry, const std::map<std::string, std::string> & map){
		std::string val= "";
		std::map<std::string, std::string>::const_iterator it_found = map.find(entry);
		if (it_found != map.end()){
			val = it_found->second;
		}
		return val;
	}

	/**
	 * Parse a string database entry, extract columns and values
	 * and return a map
	 */
	static std::map<std::string, std::string> getColumnMapFromEntry(const std::string & entry) {
		std::cout<<"getColumnMapFromEntry: "<<entry<<std::endl;
		boost::char_separator<char> sep(" ");
		std::map<std::string, std::string> entry_map;
		boost::tokenizer<boost::char_separator<char> > tokens(entry, sep);
		// forall in tokens
		{
			boost::tokenizer<boost::char_separator<char> >::const_iterator it_tokens = tokens.begin();
			const boost::tokenizer<boost::char_separator<char> >::const_iterator it_tokens_end = tokens.end();
			while (it_tokens != it_tokens_end) {
				std::string pairstr = *it_tokens;
				size_t sep_pos = pairstr.find(":");
				std::string key = pairstr.substr(0, sep_pos);
				std::string val = pairstr.substr(sep_pos+1);
				entry_map[key] = val;
				std::cout << "DatabaseObject::getColumnMapFromEntry: " << "("<<key<<","<<val <<")"<< std::endl;
				++it_tokens;
			}
		}
		return entry_map;
	}
	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const =0;

	/**
	 * Convert an templated object that can be piped to a stream to a string
	 *
	 * @param T
	 * 	The object to get a string for
	 */
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
