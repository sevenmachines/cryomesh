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
#include "common/Cycle.h"
#include <string>
#include <sstream>
namespace cryomesh {

namespace manager {

class NodeDatabaseObject: public DatabaseObject {
public:
	/**
	 * Create object from node variables
	 *
	 * @param std::string
	 * 	The uuid string of the node
	 * @param spacial::Point
	 * 	Location of the node
	 * @param Cycle
	 * 	The cycle of the entry
	 * @param double
	 * 	The activity of the node on the cycle
	 */
	NodeDatabaseObject(std::string uuid_str, spacial::Point pt, common::Cycle cyc, double act);

	/**
	 * Create object from the string of entries in the database node table
	 *
	 * @param std::string
	 * 	The string of data taken from a node entry in the database node table
	 */
	NodeDatabaseObject(const std::string & node_table_entry);

	/**
	 * Default destructor
	 */
	virtual ~NodeDatabaseObject();

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
	 * Get point variable
	 *
	 * @return spacial::Point
	 * 	The point variable
	 */
	const spacial::Point & getPoint() const;

	/**
	 * Get cycle variable
	 *
	 * @return common::Cycle
	 * 	The cycle variable
	 */
	const common::Cycle & getCycle() const;

	/**
	 * Get activity variable
	 *
	 * @return double
	 * 	The activity variable
	 */
	double getActivity() const;

	/**
	 * The string to tag ids with
	 *
	 * @var std::string
	 */
	static const std::string ID_TAG;

	/**
	 * The string to tag x coordinates with
	 *
	 * @var std::string
	 */
	static const std::string X_TAG;

	/**
	 * The string to tag y coordinates with
	 *
	 * @var std::string
	 */
	static const std::string Y_TAG;

	/**
	 * The string to tag z coordinates with
	 *
	 * @var std::string
	 */
	static const std::string Z_TAG;

	/**
	 * The string to tag activities with
	 *
	 * @var std::string
	 */
	static const std::string ACTIVITY_TAG;

	/**
	 * The string to tag cycles with
	 *
	 * @var std::string
	 */
	static const std::string CYCLE_TAG;

private:
	/**
	 * The UUID of the database object
	 *
	 * @var std::string
	 */
	std::string uuid;

	/**
	 * The Point of the database object
	 *
	 * @var Point
	 */
	spacial::Point point;

	/**
	 * The Cycle of the database object
	 *
	 * @var Cycle
	 */
	common::Cycle cycle;

	/**
	 * The activity of the database object
	 *
	 * @var std::string
	 */
	double activity;

};

}//NAMESPACE
}//NAMESPACE
#endif /* NODEDATABASEOBJECT_H_ */
