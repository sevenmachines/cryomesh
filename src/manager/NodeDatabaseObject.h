/*
 * NodeDatabaseObject.h
 *
 *  Created on: 15 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef NODEDATABASEOBJECT_H_
#define NODEDATABASEOBJECT_H_

#include "DatabaseObject.h"
#include "spacial/Point.h"
#include <string>
#include <sstream>
namespace cryomesh {

namespace manager {

class NodeDatabaseObject: public DatabaseObject {
public:
	NodeDatabaseObject(std::string uuid_str, spacial::Point point, unsigned long int cycle, double activity);

	virtual ~NodeDatabaseObject();

	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const ;

	static const std::string ID_TAG;
	static const std::string X_TAG;
	static const std::string Y_TAG;
	static const std::string Z_TAG;
	static const std::string ACTIVITY_TAG;
	static const std::string CYCLE_TAG;

};

}//NAMESPACE
}//NAMESPACE
#endif /* NODEDATABASEOBJECT_H_ */
