/**
 * Cycle.cpp
 *
 *  Created on: 1 Feb 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "Cycle.h"
#include <iostream>

namespace cryomesh {

namespace common {

Cycle::Cycle() : cycle (0) {
}
Cycle::Cycle(const long int it) :
	cycle(it) {
}

Cycle::Cycle(mpz_class mp) :
	cycle(mp) {
}

Cycle & Cycle::operator=(const Cycle & obj) {
	cycle = obj.getMP();
	return *this;
}

const Cycle Cycle::operator+(const Cycle & obj) const {
	Cycle cyc = *this;
	cyc += obj;
	return cyc;
}

Cycle & Cycle::operator+=(const Cycle & obj) {
	cycle += obj.getMP();
	return *this;
}

const Cycle Cycle::operator-(const Cycle & obj) const {
	//std::cout<<"Cycle::operator -: "<<"START"<<std::endl;
	Cycle cyc(*this);
	cyc -= obj;
#ifdef CYCLE_DEBUG
	std::cout<<"Cycle::operator -: "<<*this <<" - " <<obj << " = "<<cyc<<std::endl;
#endif
	return cyc;
}

Cycle & Cycle::operator-=(const Cycle & obj) {
	cycle -= obj.getMP();
	return *this;
}

unsigned long int Cycle::toULInt() const {
	return cycle.get_ui();
}

long int Cycle::toLInt() const {
	return cycle.get_si();
}

bool Cycle::operator==(const Cycle & obj) const {
	return this->cycle == obj.getMP();
}

bool Cycle::operator!=(const Cycle &obj) const {
	return !(*this == obj);
}

bool Cycle::operator>(const Cycle & obj) const {
	return this->cycle > obj.getMP();
}

bool Cycle::operator<(const Cycle & obj) const {
	return this->cycle < obj.getMP();
}

bool Cycle::operator>=(const Cycle & obj) const {
	return this->cycle >= obj.getMP();
}

bool Cycle::operator<=(const Cycle & obj) const {
	return this->cycle <= obj.getMP();
}

Cycle & Cycle::operator++() {
	++cycle;
	return *this;
}

Cycle Cycle::operator++(int) {
	Cycle temp = *this;
	++(*this);
	return temp;
}

Cycle Cycle::operator--(int) {
	Cycle temp = *this;
	--(*this);
	return temp;
}

Cycle & Cycle::operator--() {
	--cycle;
	return *this;
}

std::ostream& operator<<(std::ostream & os, const Cycle & obj) {
	os << obj.toLInt();
	return os;
}

const mpz_class & Cycle::getMP() const {
	return cycle;
}

}//NAMESPACE

}//NAMESPACE
