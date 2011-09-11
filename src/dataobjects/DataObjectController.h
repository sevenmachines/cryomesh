/*
 * DataObjectController.h
 *
 *  Created on: 3 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef DATAOBJECTCONTROLLER_H_
#define DATAOBJECTCONTROLLER_H_

#include "DataObject.h"

namespace cryomesh {

namespace dataobjects {

/**
 * Class used to interface with data objects
 */
template<class U, class T>
class DataObjectController {
public:

	/**
	 * Default constructor
	 */
	DataObjectController():dataObject(){
	}

	/**
	 * Contructor with size
	 *
	 * @param unsigned int
	 * 	The maximum size of the data set
	 */
	DataObjectController(unsigned int sz) {
		dataObject.setDatasetMaximumSize(sz);
	}
	virtual ~DataObjectController() {
	}
	/**
	 * Whether logging is enabled or not
	 *
	 * @param bool enable
	 * 		True to enable logging, false otherwise
	 */
	virtual void enableLogging(bool enable) {
		dataObject.enableLogging(enable);
	}

	/**
	 * Get all cycle values
	 *
	 * @return std::map<unsigned long int, double> &
	 * 		The cycle values
	 */
	virtual const std::map<U, T> &getMap() {
		refreshDataObject();
		return dataObject.getMap();
	}

	/**
	 * Get data object
	 *
	 * @return dataobjects::DataObject<U,T> &
	 * 		The data object
	 */
	virtual const dataobjects::DataObject<U, T> &getDataObject() {
		refreshDataObject();
		return dataObject;
	}

	/**
	 * Function to allow refreshing implementation if required by subclasses
	 */
	virtual void refreshDataObject() {
	}

protected:
	/**
	 * The data object for this
	 *
	 * @var DataObject
	 */
	dataobjects::DataObject<U, T> dataObject;
};

}

}

#endif /* DATAOBJECTCONTROLLER_H_ */
