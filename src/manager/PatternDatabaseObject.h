/*
 * PatternDatabaseObject.h
 *
 *  Created on: 6 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef PATTERNDATABASEOBJECT_H_
#define PATTERNDATABASEOBJECT_H_
#include "DatabaseObject.h"
#include "common/Cycle.h"
#include <string>
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace state{
	class Pattern;
}
namespace manager {

class PatternDatabaseObject: public DatabaseObject {
public:
	/**
	 * Create database object using pattern and a cycle
	 */
	PatternDatabaseObject( const std::string uuid_str, const common::Cycle & cyc, const state::Pattern & pat);

	/**
	 * Create object from the string of entries in the database output table
	 *
	 * @param std::string
	 * 	The string of data taken from a output entry in the database node table
	 */
	PatternDatabaseObject(const std::string & node_table_entry);

	/**
	 * Default destructor
	 */
	virtual ~PatternDatabaseObject();

	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const ;

	/**
	 * Get uuid variable
	 *
	 * @return std::string
	 * 	The uuid variable
	 */
	std::string getUUID() const;

	/**
	 * Get cycle variable
	 *
	 * @return common::Cycle
	 * 	The cycle variable
	 */
	const common::Cycle & getCycle() const;

	/**
	 * Get pattern variable
	 *
	 * @return Pattern
	 * 	The pattern variable
	 */
	const state::Pattern &getPattern() const ;

	/**
	 * The string to tag ids with
	 *
	 * @var std::string
	 */
	static const std::string ID_TAG;
	/**
	 * The string to tag cycles with
	 *
	 * @var std::string
	 */
	static const std::string CYCLE_TAG;

	/**
	 * The string to tag patterns with
	 *
	 * @var std::string
	 */
	static const std::string PATTERN_TAG;
private:
	/**
	 * The UUID of the database object
	 *
	 * @var std::string
	 */
	std::string uuid;

	/**
	 * The Cycle of the database object
	 *
	 * @var Cycle
	 */
	common::Cycle cycle;

	/**
	 * The Pattern of the database object
	 *
	 * @var Pattern
	 */
	boost::shared_ptr< state::Pattern > pattern;
};

}

}

#endif /* PATTERNDATABASEOBJECT_H_ */
