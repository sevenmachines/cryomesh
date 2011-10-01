/*
 * NodeMap.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef NODEMAP_H_
#define NODEMAP_H_

#include "common/KeyMappedCollection.h"
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include "Node.h"

namespace cryomesh {

namespace components {

/**
 * Helper class for NodeMap to KeyMappedCollection mapping
 */
class NodeMap: public common::KeyMappedCollection<boost::uuids::uuid, components::Node> {
public:
	/**
	 * Default constructor
	 */
	NodeMap() :
			common::KeyMappedCollection<boost::uuids::uuid, components::Node>() {
	}

	/**
	 * Default destructor
	 */
	virtual ~NodeMap() {
	}

	/**
	 * Update all entries in the nodemap
	 */
	void update() {
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end) {
				it_objects->second->update();
				++it_objects;
			}
		}
	}

	void addRandomImpulses(double positive_bias = 0.5) {
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end) {
				boost::shared_ptr<components::Impulse> random_impulse(components::Impulse::getRandom(positive_bias));
				random_impulse->setActivityDelay(0);
				it_objects->second->addImpulse(random_impulse);
				++it_objects;
			}
		}
	}

	const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > getAllInputConnections() const {
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > all_connections;

		if (objects.size() > 0) {

			// forall in objects
			{
				std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects = objects.begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects_end =
						objects.end();
				while (it_objects != it_objects_end) {
					all_connections.insert(it_objects->second->getConnector().getInputs().begin(),
							it_objects->second->getConnector().getInputs().end());
					++it_objects;
				}
			}
		}
		return all_connections;
	}

	const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > getAllOutputConnections() const {
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > all_connections;
		if (objects.size() > 0) {

			// forall in objects
			{
				std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects = objects.begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects_end =
						objects.end();
				while (it_objects != it_objects_end) {
					//	int outputs_sz = it_objects->second->getConnector().getOutputs().size();
					all_connections.insert(it_objects->second->getConnector().getOutputs().begin(),
							it_objects->second->getConnector().getOutputs().end());
					++it_objects;
				}
			}
		}
		return all_connections;
	}

	const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > getAllConnections() const {
		std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > all_connections;
		if (objects.size() > 0) {

			// forall in objects
			{
				std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects = objects.begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Node> >::const_iterator it_objects_end =
						objects.end();
				while (it_objects != it_objects_end) {
					all_connections.insert(it_objects->second->getConnector().getInputs().begin(),
							it_objects->second->getConnector().getInputs().end());
					all_connections.insert(it_objects->second->getConnector().getOutputs().begin(),
							it_objects->second->getConnector().getOutputs().end());
					++it_objects;
				}
			}
		}
		return all_connections;
	}

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const NodeMap & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const NodeMap & obj) {
		// forall in objects
		os << "NodeMap: " << "size:" << obj.getCollection().size() << std::endl;
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_objects =
					obj.getCollection().begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Node> >::const_iterator it_objects_end =
					obj.getCollection().end();
			while (it_objects != it_objects_end) {
				os << "\t" << *(it_objects->second) << std::endl;
				++it_objects;
			}
		}
		return os;
	}
};

}

}

#endif /* NODEMAP_H_ */
