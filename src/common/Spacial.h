/*
 * Spacial.h
 *
 *  Created on: 22 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef SPACIAL_H_
#define SPACIAL_H_

#include "spacial/Point.h"
#include "common/Maths.h"

namespace cryomesh {

namespace common {

class Spacial {
public:
	Spacial(bool randomise = false) :
		DEFAULT_MAX_BOUNDING_BOX(spacial::Point(100, 100, 100)), position(spacial::Point(0, 0, 0)),
				maxBoundingBox(spacial::Point(100, 100, 100)) {
		if (randomise == true) {
			this->setPosition(Spacial::getRandomPosition(this->getMaxBoundingBox()));
		}
	}

	Spacial(spacial::Point bound_box, bool randomise = false) :
		DEFAULT_MAX_BOUNDING_BOX(spacial::Point(100, 100, 100)), position(spacial::Point(0, 0, 0)),
				maxBoundingBox(bound_box) {
		if (randomise == true) {
			this->setPosition(Spacial::getRandomPosition(this->getMaxBoundingBox()));
		}
	}

	virtual ~Spacial() {

	}
	/**
	 * get the position of the object
	 *
	 * @return spacial::Point
	 * 	The spacial location of the object
	 */
	virtual const spacial::Point & getPosition() const {
		return position;
	}

	/**
	 * Set the spacial position of the object
	 *
	 * @param spacial::Point
	 * 	The position to place this object at
	 */
	virtual void setPosition(spacial::Point new_position) {
		double zero = 0.0;

		double oldx = new_position.getX();
		double maxx = DEFAULT_MAX_BOUNDING_BOX.getX();
		double oldy = new_position.getY();
		double maxy = DEFAULT_MAX_BOUNDING_BOX.getY();
		double oldz = new_position.getZ();
		double maxz = DEFAULT_MAX_BOUNDING_BOX.getZ();

		double x = common::Maths::clamp(oldx, zero, maxx);
		double y = common::Maths::clamp(oldy, zero, maxy);
		double z = common::Maths::clamp(oldz, zero, maxz);
		position = spacial::Point(x, y, z);
	}

	spacial::Point getMaxBoundingBox() const {
		return Spacial::maxBoundingBox;
	}
	void setMaxBoundingBox(const spacial::Point newbox) {
		maxBoundingBox = newbox;
	}

	static spacial::Point getRandomPosition(spacial::Point max_box) {

		double x = common::Maths::getRandomDouble(0, max_box.getX());
		double y = common::Maths::getRandomDouble(0, max_box.getY());
		double z = common::Maths::getRandomDouble(0, max_box.getZ());
		spacial::Point temp_point(x, y, z);
		return temp_point;
	}
	static boost::shared_ptr<Spacial> getRandomSpacial(spacial::Point max_box) {
		boost::shared_ptr<Spacial> temp_space(new Spacial);
		temp_space->setMaxBoundingBox(max_box);
		temp_space->setPosition(Spacial::getRandomPosition(temp_space->getMaxBoundingBox()));
		return temp_space;
	}
	const spacial::Point DEFAULT_MAX_BOUNDING_BOX;

private:
	/**
	 * Point location of the object
	 *
	 * @var spacial::Point
	 */
	spacial::Point position;

	spacial::Point maxBoundingBox;

};

}//NAMESPACE

}//NAMESPACE

#endif /* SPACIAL_H_ */
