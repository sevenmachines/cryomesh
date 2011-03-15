/*
 * ConnectionMap.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef CONNECTIONMAP_H_
#define CONNECTIONMAP_H_

#include "common/KeyMappedCollection.h"
#include "common/TimeKeeper.h"
#include "state/ActivityPattern.h"
#include "common/Cycle.h"
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include "Connection.h"

namespace cryomesh {

namespace components {

/**
 * Helper class for ConnectionMap to KeyMappedCollection mapping
 */
class ConnectionMap: public common::KeyMappedCollection<boost::uuids::uuid, components::Connection> {
public:
	ConnectionMap() {
	}
	virtual ~ConnectionMap() {
	}

	virtual void update() {
#ifdef CONNECTIONMAP_DEBUG
		std::cout << "ConnectionMap::update: " << this << std::endl;
#endif
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Connection> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				it_objects->second->update();
				++it_objects;
			}
		}
	}

	/**
	 * Get activity pattern on current cycle
	 *
	 * @ param const Cycle & cycle
	 * 		Cycle to get activity on
	 */
	boost::shared_ptr<state::ActivityPattern> getActivityPattern() const {
		return this->getActivityPattern(common::TimeKeeper::getTimeKeeper().getCycle());
	}

	/**
	 * Get activity pattern on cycle
	 *
	 * @ param const Cycle & cycle
	 * 		Cycle to get activity on
	 */
	boost::shared_ptr<state::ActivityPattern> getActivityPattern(const common::Cycle & cycle) const {
		std::list<double> all_activities;

		const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> > & all_connections =
				this->getCollection();
		// forall in all_connections
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
					it_all_connections = all_connections.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
					it_all_connections_end = all_connections.end();
			while (it_all_connections != it_all_connections_end) {
				all_activities.push_back(it_all_connections->second->getImpulses().getActivity(cycle));
				++it_all_connections;
			}
		}

		boost::shared_ptr<state::ActivityPattern> newpat(new state::ActivityPattern);
		newpat->add(all_activities);
		return newpat;
	}

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const ConnectionMap & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const ConnectionMap & obj) {
		os << "ConnectionMap: " << "size:" << obj.getCollection().size() << std::endl;
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator it_objects =
					obj.getCollection().begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<components::Connection> >::const_iterator
					it_objects_end = obj.getCollection().end();
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

#endif /* CONNECTIONMAP_H_ */
