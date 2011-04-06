/*
 * PatternDatabaseObject.cpp
 *
 *  Created on: 6 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "PatternDatabaseObject.h"
#include "NodeDatabaseObject.h"
#include "state/Pattern.h"

namespace cryomesh {

namespace manager {
const std::string PatternDatabaseObject::ID_TAG = "id";
const std::string PatternDatabaseObject::CYCLE_TAG = "cycle";
const std::string PatternDatabaseObject::PATTERN_TAG = "pattern";

PatternDatabaseObject::PatternDatabaseObject(const std::string uuid_str, const common::Cycle & cyc, const state::Pattern & pat) : uuid(uuid_str),
	cycle(cyc), pattern( new state::Pattern(pat)) {
	columns[ID_TAG] = this->uuid;
	columns[CYCLE_TAG] = toString<unsigned long int> (cycle.toULInt());
	columns[PATTERN_TAG] =pattern->getString();
}

PatternDatabaseObject::PatternDatabaseObject(const std::string & node_table_entry) {
	std::map<std::string, std::string> entry_map = DatabaseObject::getColumnMapFromEntry(node_table_entry);
	std::string uuid_str = DatabaseObject::findValue(ID_TAG, entry_map);
	std::string pattern_str = DatabaseObject::findValue(PATTERN_TAG, entry_map);
	std::string cycle_str = DatabaseObject::findValue(CYCLE_TAG, entry_map);

	this->uuid = uuid_str;
	long int cycleint = (cycle_str == "") ? 0 : atol(cycle_str.c_str());
	this->cycle = common::Cycle(cycleint);
	this->pattern = boost::shared_ptr< state::Pattern >(new state::Pattern(pattern_str));
}

PatternDatabaseObject::~PatternDatabaseObject() {

}

std::string PatternDatabaseObject::getInsert(const std::string & table) const {
	//"insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)"
	std::stringstream ss;
	ss << "insert into " << table << " (" << ID_TAG << ", " << CYCLE_TAG << ", " << PATTERN_TAG << ") ";
	ss << " values (" << "\'" << getKey(ID_TAG) << "\'" << ", "<< getKey(CYCLE_TAG)
			<< ", " << getKey(PATTERN_TAG) << ");";
	return ss.str();

}

std::string PatternDatabaseObject::getUUID() const {
	return uuid;
}
const common::Cycle & PatternDatabaseObject::getCycle() const {
	return cycle;
}

const state::Pattern &PatternDatabaseObject::getPattern() const {
	return *pattern;
}
}//NAMESPACE

}//NAMESPACE

