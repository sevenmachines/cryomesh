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
	/**
	 * Constructor
	 *
	 * 		Constructor for Tagged
	 */
	Tagged() {
		uuid = boost::uuids::random_generator()();

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
	 * Get the key of object
	 *
	 * @return boost::uuids::uuid
	 * 		The unique uuid tag of this object
	 */
	boost::uuids::uuid getKey() const {
		return this->getUUID();
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

private:
	/*
	 * Unique uuid of this object
	 *
	 * @var boost::uuids::uuid
	 */
	boost::uuids::uuid uuid;
};

}

}

#endif /* TAGGED_H_ */
