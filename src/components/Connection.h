/**
 * Connection.h
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Node.h"
#include "common/Connector.h"
#include "common/Tagged.h"
#include "manager/DatabaseObject.h"

namespace cryomesh {

namespace components {

/**
 * Connection class to manage the transfer of Impulses between Nodes
 *
 * A Connection represents the 'journey' made by Impulses as they travel
 * between a start Node and an end Node. They can be spatially based or
 * more abstract representations of this journey. As each clock moment passes
 * Impulses are propagated in some way 'closer' to their end Node.
 * Impulses can be altered by the Mesh on each cycle.
 * When Impulses reach the 'end' of their journey they are passed to the
 * end Node for accumulation.
 * Connections can also be bi-directional, an Impulse from a start Node could
 * be in some way 'reflected' back to that node. Perhaps once in a
 * 'weighted' reflection to be accumulated by the start Node, or even in
 * a constant per-cycle reflection as the original Impulse 'moves along'
 * the Connection
 */
class Connection: public common::Tagged {
public:
	/**
	 * Constructor for Connection
	 *
	 * 		Constructor
	 */
	Connection();

	/**
	 * Destructor for Connection
	 *
	 * 		Destructor
	 */
	virtual ~Connection();

	/**
	 * Update the Connection
	 *
	 * 		Update our collection of impulses. If any reach the 'endpoint' of the connection
	 * 		then pass them on to our end Nodes
	 */

	virtual void update();

	/**
	 * Get the Connector object for this Connection input
	 *
	 * @return common::Connector<Connection, Node>
	 * 		The connector for this Connection
	 */
	virtual const common::Connector<Connection, Node> & getConnector() const;

	/**
	 * Get the mutable Connector object for this Connection
	 *
	 * @return common::Connector<Connection, Node>
	 * 		The connector for this Connection
	 */
	virtual common::Connector<Connection, Node> & getMutableConnector();

	/**
	 * Add an Impulse to this connection
	 *
	 * @param boost::shared_ptr<Impulse> impulse
	 * 		The Impulse to add
	 *
	 * @return boost::shared_ptr<Impulse>
	 * 		The Impulse added, null if none added
	 */
	boost::shared_ptr<Impulse> add(boost::shared_ptr<Impulse> impulse);

	// TODO Creating shared_ptr from reference causes deletion issues
	/**
	 * Add an Impulse to this connection
	 *
	 * @param Impulse & impulse
	 * 		The Impulse to add
	 *
	 * @return boost::shared_ptr<Impulse>
	 * 		The Impulse added, null if none added
	 boost::shared_ptr<Impulse> add(Impulse & impulse);
	 */

	/**
	 * Remove an Impulse from this connection
	 *
	 * @param boost::shared_ptr<Impulse> impulse
	 * 		The impulse to remove
	 *
	 * @return boost::shared_ptr<Impulse>
	 * 		The Impulse removed, null if none removed
	 */
	boost::shared_ptr<Impulse> remove(boost::shared_ptr<Impulse> impulse);

	/**
	 * Remove an Impulse from this connection
	 *
	 * @param Impulse & impulse
	 * 		The impulse to remove
	 *
	 * @return boost::shared_ptr<Impulse>
	 * 		The Impulse removed, null if none removed
	 */
	boost::shared_ptr<Impulse> remove(Impulse & impulse);

	/**
	 * Get the impulse collection
	 *
	 * @return const ImpulseCollection &
	 * 		The impulse collection
	 */
	const ImpulseCollection & getImpulses() const;

	/**
	 * Get the mutable impulse collection
	 *
	 * 	@return ImpulseCollection &
	 * 		The mutable impulse collection
	 */
	ImpulseCollection & getMutableImpulses();

	/**
	 * Get active impulse collection
	 *
	 * @return ActiveImpulseCollection &
	 * 		The collection of active impulses
	 *
	 const ActiveImpulseCollection & getActiveImpulses() const;

	 /**
	 * Get mutable active impulse collection
	 *
	 * @return ActiveImpulseCollection &
	 * 		The collection of active impulses
	 *
	 ImpulseCollection & getActiveImpulses();
	 */

	/**
	 * The get activity timer for this object
	 *
	 * @return boost::shared_ptr<ActivityTimer>
	 */
	const boost::shared_ptr<components::ActivityTimerDistance> getActivityTimer() const;

	/**
	 * The get mutable activity timer for this object
	 *
	 * @return boost::shared_ptr<ActivityTimer>
	 */
	boost::shared_ptr<components::ActivityTimerDistance> getMutableActivityTimer();

	/**
	 * Return a database object for this object
	 *
	 * @return DatabaseObject
	 */
	boost::shared_ptr<manager::DatabaseObject> getDatabaseObject() const;

	/**
	 * Update position
	 */
	void updatePosition();

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Connection & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Connection & obj);

protected:

	/*
	 * The Connector Object for the Connection. Contains both input and
	 * output nodes
	 *
	 * @var boost::shared_ptr< common::Connector<Connection, Node> >
	 */
	boost::shared_ptr<common::Connector<Connection, Node> > connector;

	/**
	 * The collection of  impulses
	 *
	 * @var ImpulseCollection
	 */
	ImpulseCollection impulses;

	/**
	 * The default activity timer for this object
	 *
	 * @var boost::shared_ptr<ActivityTimer>
	 */
	boost::shared_ptr<components::ActivityTimerDistance> activityTimer;
};

}

}

#endif /* CONNECTION_H_ */
