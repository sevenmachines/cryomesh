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
	NodeDatabaseObject(int id, spacial::Point point, unsigned long int cycle, double activity) {
		columns[ID_TAG] = toString<int> (id);
		columns[X_TAG] = toString<double> (point.getX());
		columns[Y_TAG] = toString<double> (point.getY());
		columns[Z_TAG] = toString<double> (point.getZ());
		columns[CYCLE_TAG] = toString<int> (cycle);
		columns[ACTIVITY_TAG] = toString<int> (activity);
	}
	virtual ~NodeDatabaseObject() {
	}
	/**
	 * Get the string that can be used to insert the sql data
	 *
	 * @return
	 * 	the sql command string to insert into this table
	 */
	virtual std::string getInsert(const std::string & table) const {
		//"insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)"
		std::stringstream ss;
		ss << "insert into " << table << " (" << ID_TAG << ", " << X_TAG << ", " << Y_TAG << ", " << Z_TAG << ", "
				<< CYCLE_TAG << ", " << ACTIVITY_TAG << ") ";
		ss << " values (" << getKey(ID_TAG) << ", " << getKey(X_TAG) << ", " << getKey(Y_TAG) << ", " << getKey(Z_TAG)
				<< ", " << getKey(CYCLE_TAG) << ", " << getKey(ACTIVITY_TAG) << ");";
		return ss.str();

	}
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
