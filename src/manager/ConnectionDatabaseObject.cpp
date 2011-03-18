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

ConnectionDatabaseObject::ConnectionDatabaseObject(const std::string & uuid_str, const std::string & innode_uuid_str,
		const std::string & outnode_uuid_str, const common::Cycle & cyc, const int impulse_count) :
	uuid(uuid_str), inputNodeUUID(innode_uuid_str), outputNodeUUID(outnode_uuid_str), cycle(cyc),
			impulseCount(impulse_count) {
	columns[ID_TAG] = uuid_str;
	columns[INPUT_ID_TAG] = innode_uuid_str;
	columns[OUTPUT_ID_TAG] = outnode_uuid_str;
	columns[CYCLE_TAG] = toString<long int> (cycle.toLInt());
	columns[IMPULSE_COUNT_TAG] = toString<int> (impulse_count);
}

ConnectionDatabaseObject::ConnectionDatabaseObject(const std::string & connection_table_entry) {
	// TODO ConnectionDatabaseObject(const std::string & connection_table_entry)

	std::map<std::string, std::string> entry_map = DatabaseObject::getColumnMapFromEntry(connection_table_entry);
	std::string id_str = DatabaseObject::findValue("id", entry_map);
	std::string inputid_str = DatabaseObject::findValue("inputid", entry_map);
	std::string outputid_str = DatabaseObject::findValue("outputid", entry_map);
	std::string impulses_str = DatabaseObject::findValue("impulses", entry_map);
	std::string cycle_str = DatabaseObject::findValue("cycle", entry_map);

	this->uuid = id_str;
	this->inputNodeUUID = inputid_str;
	this->outputNodeUUID = outputid_str;
	this->impulseCount = (impulses_str == "") ? 0 : atoi(impulses_str.c_str());
	long int cycleint = (cycle_str == "") ? 0 : atol(cycle_str.c_str());
	this->cycle = common::Cycle(cycleint);

	/*
	std::cout << "ConnectionDatabaseObject::ConnectionDatabaseObject: " << "FOUND:" << std::endl;
	std::cout << "\t" << "id_str" << "=" << id_str << std::endl;
	std::cout << "\t" << "inputid_str" << "=" << inputid_str << std::endl;
	std::cout << "\t" << "outputid_str" << "=" << outputid_str << std::endl;
	std::cout << "\t" << "cycle_str" << "=" << cycle_str << std::endl;
	std::cout << "\t" << "impulses_str" << "=" << impulses_str << std::endl;

	std::cout << "ConnectionDatabaseObject::ConnectionDatabaseObject: " << "FOUND:" << std::endl;
	std::cout << "\t" << "uuid" << "=" << uuid << std::endl;
	std::cout << "\t" << "inputNodeUUID" << "=" << inputNodeUUID << std::endl;
	std::cout << "\t" << "outputNodeUUID" << "=" << outputNodeUUID << std::endl;
	std::cout << "\t" << "impulseCount" << "=" << impulseCount << std::endl;
	std::cout << "\t" << "cycle" << "=" << cycle << std::endl;
	*/
}

ConnectionDatabaseObject::~ConnectionDatabaseObject() {
}

std::string ConnectionDatabaseObject::getInsert(const std::string & table) const {
	//"insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)"
	std::stringstream ss;
	ss << "insert into " << table << " (" << ID_TAG << ", " << INPUT_ID_TAG << ", " << OUTPUT_ID_TAG << ", "
			<< CYCLE_TAG << ", " << IMPULSE_COUNT_TAG << ") ";
	ss << " values (" << "\'" << getKey(ID_TAG) << "\'" << ", " << "\'" << getKey(INPUT_ID_TAG) << "\'" << ", " << "\'"
			<< getKey(OUTPUT_ID_TAG) << "\'" << ", " << getKey(CYCLE_TAG) << ", " << getKey(IMPULSE_COUNT_TAG) << ");";
	return ss.str();

}

const std::string & ConnectionDatabaseObject::getUUID() const {
	return uuid;
}

const std::string & ConnectionDatabaseObject::getInputNodeUUID() const {
	return inputNodeUUID;
}

const std::string & ConnectionDatabaseObject::getOutputNodeUUID() const {
	return outputNodeUUID;
}

const common::Cycle & ConnectionDatabaseObject::getCycle() const {
	return cycle;
}

const int & ConnectionDatabaseObject::getImpulseCount() const {
	return impulseCount;
}
}//NAMESPACE

}//NAMESPACE
