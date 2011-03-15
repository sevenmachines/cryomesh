/*
 * Radian.cpp
 *
 *  Created on: 4 May 2010
 *      Author: niall
 */

#include "Radian.h"

namespace cryomesh {

namespace spacial {

const double Radian::PI = 3.14159265;

Radian::Radian(double d) :
	value(d) {
	// TODO Auto-generated constructor stub

}

Radian::~Radian() {
	// TODO Auto-generated destructor stub
}
double Radian::getValue() const {
	return value;
}
double Radian::toDegrees() const {
	return (value * 180 / Radian::PI);
}

bool Radian::operator==(const Radian & obj) const {
	return (this->value == obj.value);
}
Radian & Radian::operator=(const Radian & obj) {
	this->value = obj.value;
	return *this;
}
Radian Radian::operator+(const Radian & obj) const {
	return Radian(this->value + obj.value);
}
Radian Radian::operator-(const Radian & obj) const {
	return Radian(this->value - obj.value);

}

}

}
