/**
 * Connector.h
 *
 *  Created on: 19 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 *
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "common/Cycle.h"

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include "common/Misc.h"

namespace cryomesh {

namespace common {

/**
 * Connector is a template to add connectable functionality between two classes
 *
 *		Represents a template to add connectable funtionality between class U as the central
 *	connectable and class T as those objects it can be connected to
 */
template<class U, class T>
class Connector {
public:
	Connector(const unsigned int max_inputs = 0, const unsigned int max_outputs = 0) :
		maxInputs(max_inputs), maxOutputs(max_outputs) {
	}

	virtual ~Connector() {

	}

	/**
	 * Connect a unit to this one as an input
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object to be connected as input
	 *
	 * @return
	 *   true if connection succeeds, false otherwise
	 *
	 */
	bool connectInput(const boost::shared_ptr<T> obj) {
		bool this_success = false;
		boost::shared_ptr<T> obj_returned = this->connect(obj, minputs, maxInputs);
		if (obj_returned != 0) {
			this_success = true;
		}
		return (this_success);
	}

	/**
	 * Connect a list of units to this one as inputs
	 *
	 * @param std::vector<boost::shared_ptr<T> >  list
	 *   List of pointers to objects to be connected as inputs
	 *
	 * @return
	 *   true if all connections succeed, false otherwise
	 *
	 */
	bool connectInputs(const std::vector<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->connectInput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Connect an initialiser list of units to this one as inputs
	 *
	 * @param std::initializer_list<boost::shared_ptr< T > >  list
	 *   Initialiser list of pointers to objects to be connected as input
	 *
	 * @return
	 *   true if all connections succeed, false otherwise
	 */
	bool connectInputs(const std::initializer_list<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->connectInput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Connect a unit to this one as an output
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object to be connected as output
	 *
	 * @return
	 *   true if connection succeeds, false otherwise
	 *
	 */
	bool connectOutput(const boost::shared_ptr<T> & obj) {
		bool this_success = false;
		boost::shared_ptr<T> obj_returned = this->connect(obj, moutputs, maxOutputs);
		if (obj_returned != 0) {
			this_success = true;
		}
		return (this_success);
	}

	/**
	 * Connect a list of units to this one as outputs
	 *
	 * @param std::vector<boost::shared_ptr<T> >  list
	 *   List of pointers to objects to be connected as outputs
	 *
	 * @return
	 *   true if all connections succeed, false otherwise
	 *
	 */
	bool connectOutputs(const std::vector<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->connectOutput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Connect an initialiser list of units to this one as outputs
	 *
	 * @param std::initializer_list<boost::shared_ptr< T > >  list
	 *   Initialiser list of pointers to objects to be connected as outputs
	 *
	 * @return
	 *   true if all connections succeed, false otherwise
	 *
	 */
	bool connectOutputs(const std::initializer_list<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				boost::shared_ptr<T> temp_node(*it_list);
				bool success = this->connectOutput(temp_node);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an input to this unit
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object to be disconnected from input
	 *
	 * @return
	 *   true if disconnection succeeds, false otherwise
	 *
	 */
	bool disconnectInput(const boost::shared_ptr<T> & obj) {
		bool this_success = false;
		boost::shared_ptr<T> obj_returned = this->disconnect(obj, minputs);
		if (obj_returned != 0) {
			this_success = true;
		}
		return (this_success);
	}

	/**
	 * Disconnect an input to this unit
	 *
	 * @param boost::uuids::uuid id
	 *   The unique identifier of the object to be disconnected
	 *
	 * @return
	 *   true if disconnection succeeds, false otherwise
	 *
	 */
	bool disconnectInput(const boost::uuids::uuid & id) {
		bool this_success = false;

		boost::shared_ptr<T> obj_returned = this->disconnect(id, minputs);
		if (obj_returned != 0) {
			this_success = true;
		} else {
			return false;
		}
		return (this_success);
	}

	/**
	 * Disconnect a list of input units from this one
	 *
	 * @param std::vector<boost::shared_ptr<T> >  list
	 *   List of pointers to objects to be disconnected
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectInputs(const std::vector<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectInput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an initialiser list of input units from this one
	 *
	 * @param std::initializer_list<boost::shared_ptr< T > >  list
	 *   Initialiser list of pointers to objects to be disconnected as inputs
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectInputs(const std::initializer_list<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectInput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect a list of input units from this one
	 *
	 * @param boost::uuids::uuid  list
	 *   List of uuids to objects to be disconnected
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectInputs(const std::vector<boost::uuids::uuid> & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectInput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an initialiser list of input units from this one
	 *
	 * @param boost::uuids::uuid  list
	 *   Initialiser list of uuids to objects to be disconnected as inputs
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectInputs(const std::initializer_list<boost::uuids::uuid> & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::uuids::uuid>::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::uuids::uuid>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				boost::uuids::uuid temp_uuid = *it_list;
				bool success = this->disconnectInput(temp_uuid);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect all input units from this one
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectAllInputs() {
		bool overall_success = true;
		// forall in list
		{
			typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::iterator it_list = minputs.begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_list_end =
					minputs.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectInput(it_list->first);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an output to this unit
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object to be disconnected from output
	 *
	 * @return
	 *   true if disconnection succeeds, false otherwise
	 *
	 */
	bool disconnectOutput(const boost::shared_ptr<T> & obj) {
		bool this_success = false;

		boost::shared_ptr<T> obj_returned = this->disconnect(obj, moutputs);
		if (obj_returned != 0) {
			this_success = true;
		}
		return (this_success);

	}

	/**
	 * Disconnect an output to this unit
	 *
	 * @param boost::uuids::uuid id
	 *   The unique identifier of the object to be disconnected
	 *
	 * @return
	 *   true if disconnection succeeds, false otherwise
	 *
	 */
	bool disconnectOutput(const boost::uuids::uuid & id) {
		bool this_success = false;
		boost::shared_ptr<T> obj_returned = this->disconnect(id, moutputs);
		if (obj_returned != 0) {
			this_success = true;
		} else {
			return false;
		}
		return (this_success);

	}

	/**
	 * Disconnect a list of Output units from this one
	 *
	 * @param std::vector<boost::shared_ptr<T> >  list
	 *   List of pointers to objects to be disconnected
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectOutputs(const std::vector<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectOutput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an initialiser list of Output units from this one
	 *
	 * @param std::initializer_list<boost::shared_ptr< T > >  list
	 *   Initialiser list of pointers to objects to be disconnected as Outputs
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectOutputs(const std::initializer_list<boost::shared_ptr<T> > & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				boost::shared_ptr<T> temp_node(*it_list);
				bool success = this->disconnectOutput(temp_node);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect a uuid list of Output units from this one
	 *
	 * @param boost::uuids::uuid  list
	 *   List of uuids to objects to be disconnected
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectOutputs(const std::vector<boost::uuids::uuid> & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::vector<boost::shared_ptr<T> >::const_iterator it_list = list.begin();
			const typename std::vector<boost::shared_ptr<T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectOutput(*it_list);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect an uuid initiaialiser list of Output units from this one
	 *
	 * @param boost::uuids::uuid  list
	 *   Initialiser list of uuids to objects to be disconnected as Outputs
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectOutputs(const std::initializer_list<boost::uuids::uuid> & list) {
		bool overall_success = true;
		// forall in list
		{
			typename std::initializer_list<boost::uuids::uuid>::const_iterator it_list = list.begin();
			const typename std::initializer_list<boost::uuids::uuid>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				boost::uuids::uuid temp_uuid = *it_list;
				bool success = this->disconnectOutput(temp_uuid);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	/**
	 * Disconnect all Output units from this one
	 *
	 * @return
	 *   true if all disconnections succeed, false otherwise
	 *
	 */
	bool disconnectAllOutputs() {
		bool overall_success = true;
		// forall in list
		{
			typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::iterator it_list = moutputs.begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_list_end =
					moutputs.end();
			while (it_list != it_list_end) {
				bool success = this->disconnectOutput(it_list->first);
				overall_success = overall_success && success;
				++it_list;
			}
		}
		return overall_success;
	}

	// GET/SET
	/**
	 * Get all inputs
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr<T>
	 *   The map of inputs
	 *
	 */
	const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & getInputs() const {
		return minputs;
	}

	/**
	 * Get all inputs as mutable object
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr<T>
	 *   The map of inputs
	 *
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<T> > & getMutableInputs() {
		return minputs;
	}

	/**
	 * Get all outputs
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr<T>
	 *   The map of outputs
	 *
	 */
	const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & getOutputs() const {
		return moutputs;
	}

	/**
	 * Get all outputs as mutable object
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr<T>
	 *   The map of outputs
	 *
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<T> > & getMutableOutputs() {
		return moutputs;
	}

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Connector<U,T> & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Connector<U, T> & obj) {
		// TODO: friend std::ostream& operator<<(std::ostream & os, const Connector<U,T> & obj)
		os << "Connector<>:" << obj << std::endl;
		os << "\tInputs:" << std::endl;
		// forall in minputs
		{
			typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_minputs =
					obj.getInputs().begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_minputs_end =
					obj.getOutputs().end();
			while (it_minputs != it_minputs_end) {
				os << "\tUUID:" << common::Misc::print(os, it_minputs->first);
				++it_minputs;
			}
		}
		os << "\tOutputs:" << std::endl;

		// forall in moutputs
		{
			typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_moutputs =
					obj.getOutputs().begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_moutputs_end =
					obj.getOutputs().end();
			while (it_moutputs != it_moutputs_end) {
				os << "\tUUID:" << common::Misc::print(os, it_moutputs->first);
				++it_moutputs;
			}
		}
		return os;
	}

protected:

	/**
	 * Connect up an object using the supplied map
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object that is to be connected
	 * @param std::map<boost::uuids::uuid, boost::shared_ptr<T> >  objs
	 *   The map that will be used to connect the object
	 *@param unsinged int
	 *		Maximum connections to allow
	 *
	 * @return
	 *   Pointer to the connected object
	 *
	 */
	boost::shared_ptr<T> connect(const boost::shared_ptr<T> obj,
			std::map<boost::uuids::uuid, boost::shared_ptr<T> > & objs, unsigned int max_connections) {
		boost::uuids::uuid temp_id = obj->getUUID();

		if (max_connections > 0) {
			if (objs.size() < max_connections) {
				objs[temp_id] = obj;
			} else {
				std::cout << "Connector::connect: " << "Warning, ignoring attempt to add to a full container. ("
						<< objs.size() << " of " << max_connections << std::endl;
			}
		}else{
			objs[temp_id] = obj;

		}
		return obj;
}

	/**
	 * Disconnect an object using the supplied map
	 *
	 * @param boost::shared_ptr<T> obj
	 *   Pointer to the object that is to be disconnected
	 * @param std::map<boost::uuids::uuid, boost::shared_ptr<T> >  objs
	 *   The map that will be used to disconnect the object
	 *
	 * @return
	 *   Pointer to the disconnected object, pointer is 0 is object was not found
	 *
	 */
	boost::shared_ptr<T> disconnect(const boost::shared_ptr<T> obj,
			std::map<boost::uuids::uuid, boost::shared_ptr<T> > & objs) {
		boost::shared_ptr<T> deleted_obj;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::iterator it_found = objs.find(obj->getUUID());
		if (it_found != objs.end()) {
			deleted_obj = it_found->second;
			objs.erase(it_found);
		}
		return deleted_obj;
	}

	/**
	 * Disconnect an object using the supplied map
	 *
	 * @param boost::uuids::uuid  id
	 *   The uuid of the object that is to be disconnected
	 * @param std::map<boost::uuids::uuid, boost::shared_ptr<T> > & objs
	 *   The map that will be used to disconnect the object
	 *
	 * @return
	 *   Pointer to the disconnected object, pointer is null is object was not found
	 *
	 */
	boost::shared_ptr<T> disconnect(const boost::uuids::uuid & id,
			std::map<boost::uuids::uuid, boost::shared_ptr<T> > & objs) {
		boost::shared_ptr<T> deleted_obj;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::iterator it_found = objs.find(id);
		if (it_found != objs.end()) {
			deleted_obj = it_found->second;
			objs.erase(it_found);
		}
		return deleted_obj;
	}

	/**
	 * uuid mapping of inputs
	 *
	 * @var std::map<boost::uuids::uuid, boost::shared_ptr<T> >
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<T> > minputs;

	/**
	 * uuid mapping of outputs
	 *
	 * @var std::map<boost::uuids::uuid, boost::shared_ptr<T> >
	 */
	std::map<boost::uuids::uuid, boost::shared_ptr<T> > moutputs;

	/**
	 * Max number of inputs
	 *
	 * @var unsigned int
	 */
	unsigned int maxInputs;

	/**
	 * Max number of outputs
	 *
	 * @var unsigned int
	 */
	unsigned int maxOutputs;

};

}

}

#endif /* CONNECTOR_H_ */
