/**
 * Node.h
 *
 *  Created on: 3 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef NODE_H_
#define NODE_H_

#include "ImpulseCollection.h"

#include "common/Cycle.h"
#include "common/Connector.h"
#include "common/Tagged.h"
#include "common/Defs.h"
#include "spacial/Point.h"
#include "dataobjects/DataObjectController.h"
#include "manager/NodeDatabaseObject.h"


#include <list>
#include <map>

namespace cryomesh {

namespace components {

class Connection;

/**
 * Node is an accumulation and computational nodal point of impulses
 *
 * A Node represents the end point of one or many connections. Here,
 * Impulses are accumulated and new Impulses generated depending
 * on some determining criteria
 */
class Node: public common::Tagged, public dataobjects::DataObjectController<unsigned long int, double> {
public:

	/**
	 * Enum representing posible activation states
	 */
	enum ActivationState {
		Positive, Negative, None
	};

	/**
	 * Contructor
	 *
	 * Contructor for Node
	 */
	Node();

	/**
	 * Destructor
	 *
	 * Destructor for Node
	 */
	virtual ~Node();

	/*
	 * General function to start a full update cycle on the node
	 *
	 *		Update the ImpulseCollection of impulses, then check if this is
	 *		active on the current tick. If active then send an impulse to our
	 *		connections.
	 */
	virtual void update();

	/**
	 * Check if the object is ready to fire off an impulse and carry it out
	 *
	 * @return ActivationState
	 * 		Return the action that was taken
	 */
	Node::ActivationState checkFire();

	/**
	 * Check level of impulses and decide whether to activate the node
	 *
	 * @return Node::ActivationState
	 * 		Positive if activity is over threshold, negative if under -threshold, None otherwise
	 */
	virtual Node::ActivationState checkActivationState();

	/**
	 * Emit a positive impulse to outgoing connections
	 */
	virtual void emitImpulsePositive();

	/**
	 * Emit a negative impulse to outgoing connections
	 */
	virtual void emitImpulseNegative();

	/**
	 * Add incoming Impulse
	 *
	 * @param boost::shared_ptr<Impulse> impulse
	 * 		The Impulse to add
	 * @return boost::shared_ptr<Impulse>
	 * 		The impulse added, null if none added
	 */
	virtual boost::shared_ptr<Impulse> addImpulse(boost::shared_ptr<Impulse> impulse);

	/**
	 * Add a list of incoming Impulses
	 *
	 * @param std::list<boost::shared_ptr<Impulse> > impulses
	 * 		The Impulses to add
	 */
	virtual void addImpulses(std::list<boost::shared_ptr<Impulse> > impulses);

	/**
	 * Get the Connector object for this Node
	 *
	 * @return const common::Connector<Node, Connection> &
	 * 		The Connector for this object
	 */
	const common::Connector<Node, Connection> & getConnector() const;

	/**
	 * Get the mutable Connector object for this Node
	 *
	 * @return common::Connector<Node, Connection> &
	 * 		The mutable Connector for this object
	 */
	common::Connector<Node, Connection> & getMutableConnector();

	/**
	 * Get the collection of Impulses for this Node
	 *
	 * @return const ImpulseCollection &
	 * 		The collection of Impulses for this Node
	 */
	const ImpulseCollection & getImpulses() const;

	/** Get the Impulse that is emitted
	 *
	 * @return const boost::shared_ptr< Impulse >
	 * 		The emitted Impulse
	 */
	const boost::shared_ptr<Impulse> getEmittedImpulse() const;

	/** Get the mutable Impulse that is emitted
	 *
	 * @return boost::shared_ptr< Impulse >
	 * 		The mutable emitted Impulse
	 */
	boost::shared_ptr<Impulse> getMutableEmittedImpulse();

	/**
	 * Get the mutable collection of Impulses for this Node
	 *
	 * @return ImpulseCollection &
	 * 		The mutable collection of Impulses for this Node
	 */
	ImpulseCollection & getMutableImpulses();

	/**
	 * Get the collection of all activities
	 *
	 * @return std::list<double> &
	 * 		List of activities
	 */
	const std::map<common::Cycle, double> & getActivities() const;

	/**
	 * Update and get the current activity of the node
	 *
	 * @return double
	 * 		The current activity
	 */
	double updateActivity();

	/**
	 * Update and get the  activity of the node on specific cycle
	 *
	 * @return double
	 * 		The current activity
	 */
	double updateActivity(const common::Cycle & cycle);

	/**
	 * Get the current activity of the node
	 *
	 * @return double
	 * 		The current activity
	 */
	double getActivity() const;

	/**
	 * Get the  activity of the node on specific cycle
	 *
	 * @return double
	 * 		The current activity
	 */
	double getActivity(const common::Cycle & cycle) const;

	/**
	 * Set the current activity of the node
	 *
	 * @param double
	 * 		The current activity value to be set
	 *
	 * @return double
	 * 		The activity set
	 */
	double setActivity(double activity);

	/**
	 * Set the activity at cycle of the node
	 *
	 *@param const Cycle & cycle
	 *		The cycle the activty is on
	 * @param double
	 * 		The current activity value to be set
	 *
	 * @return double
	 * 		The activity set
	 */
	double setActivity(const common::Cycle & cycle, double activity);

	/**
	 * Return a database object for this node
	 *
	 * @return DatabaseObject
	 */
	boost::shared_ptr< manager::DatabaseObject > getDatabaseObject()const;

	/**
	 * get the position of the node
	 *
	 * @return spacial::Point
	 * 	The spacial location of the node
	 */
	const spacial::Point & getPosition() const ;

	/**
	 * Set the spacial position of the node, remembering to update connections lengths
	 *
	 * @param spacial::Point
	 * 	The position to place this node at
	 */
	void setPosition(const spacial::Point & new_position );

	/**
	 * Randomise the nodes state
	 */
	void randomise();

/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Node & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Node & obj);

	// STATICS

	/**
	 * The maximum number of stored activities
	 *
	 * @var const int
	 */
	static const int MAX_ACTIVITIES_LENGTH;

	/**
	 * The activity threshold which triggers an impulse
	 *
	 * @var double
	 */
	static const double ACTIVITY_THRESHOLD;

	/*
	 * The default max Point for the bounding box
	 *
	 * @ var Point
	 */
	static const spacial::Point MAX_BOUNDING_BOX_POINT;

	/*
	 * Get a random node within specified bounding box
	 *
	 * @param Point
	 * 	The max bounding box point, default to predefined max
	 *
	 * 	@return boost::shared_ptr<Node>
	 * 		The randomised node
	 */
	static boost::shared_ptr< Node > getRandom(const spacial::Point  & max_point = MAX_BOUNDING_BOX_POINT);

protected:

	/**
	 * Update the collection of impulses by one cycle
	 */
	virtual void updateImpulses();

	/**
	 * Emit an impulse to outgoing connections
	 *
	 * @param bool positive
	 * 		Is the impulse to be emitted positive or negative
	 */
	virtual void emitImpulse(bool positive);

	/**
	 * Add an activity to the list of activities
	 *
	 *	@param Cycle cycle
	 *		The cycle this activity is on
	 * @param double activity
	 * 		The activity to add
	 *
	 * @return
	 * 		The current activity
	 */
	virtual double addActivity(common::Cycle, double activity);

	/**
	 * Recalculate state of node and connections based on current position
	 *
	 */
	virtual void updatePosition();


private:
	/**
	 * The Connector Object for the Node
	 *
	 * @var boost::shared_ptr< common::Connector<Node, Connection> >
	 */
	boost::shared_ptr<common::Connector<Node, Connection> > connector;

	/**
	 * Collection of all Impulses held by this object
	 *
	 * @var ImpulseCollection
	 */
	ImpulseCollection impulses;

	/**
	 * Impulse to emit
	 *
	 * @var boost::shared_ptr<Impulse>
	 */
	boost::shared_ptr<Impulse> emittedImpulse;

	/**
	 * Map of Cycle to activity
	 *
	 * @var std::map<Cycle, double>
	 */
	dataobjects::DataObject<common::Cycle, double> activities;

	/**
	 * Point location of the node
	 *
	 * @var spacial::Point
	 */
	spacial::Point position;

};

}

}

#endif /* NODE_H_ */
