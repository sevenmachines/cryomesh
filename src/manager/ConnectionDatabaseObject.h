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
#include <string>
#include <sstream>

namespace cryomesh {

namespace manager {

class ConnectionDatabaseObject : public DatabaseObject{
public:
	ConnectionDatabaseObject(std::string uuid_str, std::string innode_uuid_str, std::string outnode_uuid_str, unsigned long int cycle, int impulse_count);
	virtual ~ConnectionDatabaseObject();

	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const;

	static const std::string ID_TAG;
	static const std::string INPUT_ID_TAG;
	static const std::string OUTPUT_ID_TAG;
	static const std::string IMPULSE_COUNT_TAG;
	static const std::string CYCLE_TAG;
};

}//NAMESPACE

}//NAMESPACE

#endif /* CONNECTIONDATABASEOBJECT_H_ */
