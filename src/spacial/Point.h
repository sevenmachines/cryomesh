/*
 * Point.h
 *
 *  Created on: 31 Mar 2010
 *      Author: niall
 */

#ifndef POINT_H_
#define POINT_H_

#include <iostream>

namespace cryomesh {

namespace spacial {

class SphericalPoint;

class Point {
public:
	Point();
	Point ( double new_x, double new_y, double new_z);
	Point (const Point & sp);
	Point (const SphericalPoint & sp);

	virtual ~Point();
	double getDistance(const Point & start) const;
	void setToMinimumDistances(double min);
	double getX()const ;
	double getY()const;
	double getZ()const;
	/*
	 * get  a normalised vector
	 */
	Point getNormalisedVector() const;
	// hazy equals for doubles
	bool equals(const Point & obj) const;
	bool operator==(const Point & obj) const;
	Point & operator=(const Point & obj);
	Point  operator+(const Point & obj) const;
	Point  operator-(const Point & obj) const;
	Point  operator*( double d) const;

	friend std::ostream & operator<<(std::ostream& os, const Point & obj);

private:
	double x;
	double y;
	double z;

};

}

}


#endif /* POINT_H_ */
