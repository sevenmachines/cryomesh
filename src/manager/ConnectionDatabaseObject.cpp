/*
 * ConnectionDatabaseObject.cpp
 *
 *  Created on: 16 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "ConnectionDatabaseObject.h"

namespace cryomesh {

namespace manager {

const std::string ConnectionDatabaseObject::ID_TAG = "id";
const std::string ConnectionDatabaseObject::INPUT_ID_TAG = "inputid";
const std::string ConnectionDatabaseObject::OUTPUT_ID_TAG = "outputid";
const std::string ConnectionDatabaseObject::IMPULSE_COUNT_TAG = "impulses";
const std::string ConnectionDatabaseObject::CYCLE_TAG = "cycle";

ConnectionDatabaseObject::ConnectionDatabaseObject(std::string uuid_str, std::string innode_uuid_str,
		std::string outnode_uuid_str, unsigned long int cycle, int impulse_count) {
	columns[ID_TAG] = uuid_str;
	columns[INPUT_ID_TAG] = innode_uuid_str;
	columns[OUTPUT_ID_TAG] = outnode_uuid_str;
	columns[CYCLE_TAG] = toString<int> (cycle);
	columns[IMPULSE_COUNT_TAG] = toString<int> (impulse_count);
}

ConnectionDatabaseObject::~ConnectionDatabaseObject() {
}

std::string ConnectionDatabaseObject::getInsert(const std::string & table) const {
	//"insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)"
	std::stringstream ss;
	ss << "insert into " << table << " (" << ID_TAG << ", " << INPUT_ID_TAG << ", " << OUTPUT_ID_TAG << ", " << CYCLE_TAG << ", "
			<< IMPULSE_COUNT_TAG  << ") ";
	ss << " values (" << "\'" << getKey(ID_TAG) << "\'" << ", " << getKey(INPUT_ID_TAG) << ", " << getKey(OUTPUT_ID_TAG) << ", "
			<< getKey(CYCLE_TAG) << ", " << getKey(IMPULSE_COUNT_TAG) << ");";
	return ss.str();

}

}//NAMESPACE

}//NAMESPACE
