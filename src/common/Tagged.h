/*
 * Tagged.h
 *
 *  Created on: 19 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 *
 *      Class to add uuid tagged functionality to classes
 */

#ifndef TAGGED_H_
#define TAGGED_H_
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <sstream>

namespace cryomesh {

namespace common {

/**
 * Tagged class implements uuid tagging of classes
 * 
 * 		Tagged class should be inherited by functions who want to be uuid tagged
 * and have the related functionality.
 */
class Tagged {
public:

	//DEPRECATED
	/**
	 * Get a new ID, not guaranteed unique
	 *
	 * @return unsigned long int
	 * 	The new id
	 *
	 static unsigned long int getNewID() {
	 ++Tagged::ids;
	 return ids;
	 }*/

	//DEPRECATED
	/**
	 * Variable for getting new id'd
	 *
	 static unsigned long int ids;
	 */

	/**
	 * Constructor
	 *
	 * 		Constructor for Tagged
	 */
	Tagged() :
		uuid(boost::uuids::random_generator()()) {
	}

	/**
	 * Destructor
	 *
	 * 		Destructor for Tagged
	 */
	virtual ~Tagged() {

	}

	/**
	 * Get the uuid of object
	 *
	 * @return boost::uuids::uuid
	 * 		The unique uuid tag of this object
	 */
	boost::uuids::uuid getUUID() const {
		return uuid;
	}

	/**
	 * Get the uuid of object as a string
	 *
	 * @return std::string
	 * 		The unique uuid tag of this object as a string
	 */
	std::string getUUIDString() const {
		std::stringstream ss;
		ss << (this->getUUID());
		return ss.str();
	}

	/**
	 * Get the key of object
	 *
	 * @return boost::uuids::uuid
	 * 		The unique uuid tag of this object
	 */
	boost::uuids::uuid getKey() const {
		return this->getUUID();
	}

	/**
	 * Get the key string of object
	 *
	 * @return std::string
	 * 		The unique uuid tag string of this object
	 */
	std::string getKeyString() const {
		return this->getUUIDString();
	}

	/**
	 * Get the tag of object
	 *
	 * @return boost::uuids::uuid
	 * 		The unique uuid tag of this object
	 */
	boost::uuids::uuid getTag() const {
		return this->getUUID();
	}

	/**
	 * Get the tag string of object
	 *
	 * @return std::string
	 * 		The unique uuid tag string of this object
	 */
	std::string getTagString() const {
		return this->getUUIDString();
	}

	//DEPRECATED
	/**
	 * Get the ID of object
	 *
	 * @return unsigned long ing
	 * 		The not unique id tag of this object
	 *
	unsigned long int getID() const {
		return id;
	}*/

private:
	/*
	 * Unique uuid of this object
	 *
	 * @var boost::uuids::uuid
	 */
	boost::uuids::uuid uuid;

	//DEPRECATED
	/**
	 * Compatibility id, not guaranteed unique
	 *
	 * @var unsigned long int
	 *
	unsigned long int id;
	*/
};

}

}

#endif /* TAGGED_H_ */
