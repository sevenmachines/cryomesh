/*
 * ConnectionDatabaseObject.h
 *
 *  Created on: 16 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef CONNECTIONDATABASEOBJECT_H_
#define CONNECTIONDATABASEOBJECT_H_

#include "DatabaseObject.h"
#include "spacial/Point.h"
#include "common/Cycle.h"
#include <string>
#include <sstream>

namespace cryomesh {

namespace manager {

class ConnectionDatabaseObject: public DatabaseObject {
public:
	/**
	 * Create database object from connection information
	 *
	 * @param std::string
	 * 	The uuid of the connection
	 * @param std::string
	 * 	The uuid of the input node
	 * @param std::string
	 * 	The uuid of the output node
	 * @param common::Cycle
	 * 	The cycle of the entry
	 * @param int
	 * 	The impulse count of the connection on this entry
	 */
	ConnectionDatabaseObject(const std::string & uuid_str, const std::string & innode_uuid_str,
			const std::string & outnode_uuid_str, const common::Cycle & cycle, const int impulse_count);

	/**
	 * Create database object from a string database entry for a connection
	 *
	 * @param std::string
	 * 	The database entry of the connection
	 */
	ConnectionDatabaseObject(const std::string & connection_table_entry);

	/**
	 * Default Destructor
	 */
	virtual ~ConnectionDatabaseObject();

	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const;

	/**
	 * Get uuid variable of this object
	 *
	 * @return std::string
	 * 	The uuid variable
	 */
	const std::string & getUUID() const;

	/**
	 * Get inputNodeUUID variable of this object
	 *
	 * @return std::string
	 * 	The inputNodeUUID variable
	 */
	const std::string & getInputNodeUUID() const;

	/**
	 * Get outputNodeUUID variable of this object
	 *
	 * @return std::string
	 * 	The outputNodeUUID variable
	 */
	const std::string & getOutputNodeUUID() const;

	/**
	 * Get cycle variable of this object
	 *
	 * @return std::string
	 * 	The cycle variable
	 */
	const common::Cycle & getCycle() const;

	/**
	 * Get impulseCount variable of this object
	 *
	 * @return std::string
	 * 	The impulseCount variable
	 */
	const int & getImpulseCount() const;

	static const std::string ID_TAG;
	static const std::string INPUT_ID_TAG;
	static const std::string OUTPUT_ID_TAG;
	static const std::string IMPULSE_COUNT_TAG;
	static const std::string CYCLE_TAG;

private:
	std::string uuid;
	std::string inputNodeUUID;
	std::string outputNodeUUID;
	common::Cycle cycle;
	int impulseCount;
};

}//NAMESPACE

}//NAMESPACE

#endif /* CONNECTIONDATABASEOBJECT_H_ */
