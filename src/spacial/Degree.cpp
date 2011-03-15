/*
 * Degree.cpp
 *
 *  Created on: 4 May 2010
 *      Author: niall
 */

#include "Degree.h"
#include "Radian.h"
namespace cryomesh {

namespace spacial {

Degree::Degree(double d) :
	value(d) {
}

Degree::~Degree() {
}

double Degree::getValue() const {
	return value;
}
double Degree::toRadians() const {
	return (value * Radian::PI / 180);
}
}

}
