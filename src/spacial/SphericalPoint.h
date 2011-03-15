/*
 * SphericalPoint.h
 *
 *  Created on: 31 Mar 2010
 *      Author: niall
 */

#ifndef SPHERICALPOINT_H_
#define SPHERICALPOINT_H_

#include <iostream>
#include <spacial/Radian.h>
#include <spacial/Degree.h>

namespace cryomesh {

namespace spacial {
class Point;
class SphericalPoint {
public:
	SphericalPoint();
	SphericalPoint(double r, Radian  th, Radian  ph);
	SphericalPoint(double r, Degree  th, Degree  ph);
	SphericalPoint(const Point & p);
	SphericalPoint(const SphericalPoint & p);
	virtual ~SphericalPoint();

	double getArcLength(const SphericalPoint & start) const;
	double getArcLengthBasic(const SphericalPoint & start) const;
	double getArcLengthHaversine(const SphericalPoint & start) const;
	SphericalPoint getOpposite() const;

	//getters/setters
	double getRadius() const;
	Radian getTheta() const;
	Radian getPhi() const;
	void setRadius(double r) ;
	void setTheta(Radian th) ;
		void setPhi(Radian ph) ;

	bool equals(const SphericalPoint & obj)const;

	bool operator==(const SphericalPoint & obj) const;
	SphericalPoint & operator=(const SphericalPoint & obj);
	SphericalPoint operator+(const SphericalPoint & obj)const;
	SphericalPoint operator-(const SphericalPoint & obj)const;

	friend std::ostream & operator<<(std::ostream& os, const SphericalPoint & obj);

	static const double PI;
	static const double MIN_ALLOWED_X;

private:
	// r (<=1)
	double radius;
	// inclination (theta)
	Radian theta;
	// azimuth (phi)
	Radian phi;

};

}

}

#endif /* SPHERICALPOINT_H_ */
