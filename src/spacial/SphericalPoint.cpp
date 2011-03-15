/*
 * SphericalPoint.cpp
 *
 *  Created on: 31 Mar 2010
 *      Author: niall
 */

#include "SphericalPoint.h"
#include "Point.h"
#include "common/Maths.h"
#include <math.h>

namespace cryomesh {

namespace spacial {

const double SphericalPoint::MIN_ALLOWED_X = 0.00000000001;
const double SphericalPoint::PI = 3.14159265;

SphericalPoint::SphericalPoint() :
	radius(0), theta(0), phi(0) {
}
SphericalPoint::SphericalPoint(double r, Radian th, Radian ph) :
	radius(r), theta(th), phi(ph) {
}
SphericalPoint::SphericalPoint(double r, Degree th, Degree ph) :
	radius(r), theta(Radian(th.toRadians())), phi(Radian(ph.toRadians())) {

}
SphericalPoint::SphericalPoint(const SphericalPoint & p) :
	radius(p.getRadius()), theta(p.getTheta()), phi(p.getPhi()) {
}
SphericalPoint::SphericalPoint(const Point & p) :
	radius(0), theta(0), phi(0) {
	double temp_x = p.getX();
	double temp_y = p.getY();
	double temp_z = p.getZ();
	// deal with zero x
	if (temp_x == 0) {
		temp_x = MIN_ALLOWED_X;
	}
	setRadius(sqrt((temp_x * temp_x) + (temp_y * temp_y) + (temp_z * temp_z)));

	setTheta(acos(temp_z / radius));
	setPhi(atan2(temp_y, temp_x));

}
SphericalPoint::~SphericalPoint() {
	// TODO Auto-generated destructor stub
}
double SphericalPoint::getArcLength(const SphericalPoint & start) const {
	return getArcLengthHaversine(start);
}
double SphericalPoint::getArcLengthBasic(const SphericalPoint & start) const {
	// use haversine equation
	Radian new_phi = this->getPhi() - start.getPhi();
	double degrees = new_phi.toDegrees();
	double delta_phi = new_phi.getValue();
	double p1 = sin(start.getTheta().getValue()) * sin(this->getTheta().getValue());
	double p2 = cos(start.getTheta().getValue()) * cos(this->getTheta().getValue());
	double p3 = cos(delta_phi);
	double arc_length = this->getRadius() * acos(p1 + (p2 * p3));

	return arc_length;

}
double SphericalPoint::getArcLengthHaversine(const SphericalPoint & start) const {
	// use haversine equation
	//deal with opposite case
	if (start.equals( this->getOpposite())){
		return PI * this->getRadius();
	}
	double phi_f = this->getPhi().getValue();
	double phi_s = start.getPhi().getValue();

	double delta_phi = ( this->getPhi() - start.getPhi() ).getValue();
	double degrees = Radian(delta_phi).toDegrees();
	double sine_delta_phi = sin(delta_phi);
	double cos_delta_phi = cos(delta_phi);

	double sine_theta_start = sin(start.getTheta().getValue());
	double cos_theta_start = cos(start.getTheta().getValue());
	double sine_theta_this = sin(this->getTheta().getValue());
	double cos_theta_this = cos(this->getTheta().getValue());

	double p1 = (cos_theta_this * sine_delta_phi);
	double p2a = (cos_theta_start * sine_theta_this);
	double p2b = (sine_theta_start * cos_theta_this * cos_delta_phi);
	double p2 = p2a - p2b;
	double denominator = (sine_theta_start * sine_theta_this)
			+ (cos_theta_start * cos_theta_this * cos_delta_phi);

	double numerator = sqrt((p1 * p1) + (p2 * p2));

	double arc_length = this->getRadius() * atan2(numerator, denominator);

	return arc_length;


}

SphericalPoint SphericalPoint::getOpposite() const {
	return (SphericalPoint(Point(*this) * -1));

}
double SphericalPoint::getRadius() const {
	return radius;
}
Radian SphericalPoint::getTheta() const {
	return theta;
}
Radian SphericalPoint::getPhi() const {
	return phi;
}
void SphericalPoint::setRadius(double r) {
	radius = r;
}
void SphericalPoint::setTheta(Radian th) {
	theta = th;
}
void SphericalPoint::setPhi(Radian ph) {
	phi = ph;
}
bool SphericalPoint::equals(const SphericalPoint & obj) const {
	const double DELTA = 0.000001;
	bool brad = (this->getRadius() + DELTA > obj.getRadius())
			&& (this->getRadius() - DELTA < obj.getRadius());
	bool bth =
			(this->getTheta().getValue() + DELTA > obj.getTheta().getValue())
					&& (this->getTheta().getValue() - DELTA
							< obj.getTheta().getValue());
	bool bph = (this->getPhi().getValue() + DELTA > obj.getPhi().getValue())
			&& (this->getPhi().getValue() - DELTA < obj.getPhi().getValue());
	return (brad && bth && bph);
}
bool SphericalPoint::operator==(const SphericalPoint & obj) const {
	return (this->getRadius() == obj.getRadius() && this->getTheta()
			== obj.getTheta() && this->getPhi() == obj.getPhi());
}
SphericalPoint & SphericalPoint::operator=(const SphericalPoint & obj) {
	if (this != &obj) {
		this->setRadius(obj.getRadius());
		this->setTheta(obj.getTheta());
		this->setPhi(obj.getPhi());
	}
	return *this;
}
SphericalPoint SphericalPoint::operator+(const SphericalPoint & obj) const {
	// vector addition, switch to cartesian, do calculation,
	//then create a new SphericalPoint from the result
	return SphericalPoint(Point(*this) + Point(obj));
}
SphericalPoint SphericalPoint::operator-(const SphericalPoint & obj) const {
	// vector subtraction, switch to cartesian, do calculation,
	//then create a new SphericalPoint from the result
	return SphericalPoint(Point(*this) - Point(obj));
}

std::ostream & operator<<(std::ostream& os, const SphericalPoint & obj) {
	os << "SphericalPoint: " << "(" << obj.getRadius() << ", "
			<< obj.getTheta().toDegrees() << ", " << obj.getPhi().toDegrees()
			<< ")";
	return os;

}
}//NAMESPACE

}//NAMESPACE

