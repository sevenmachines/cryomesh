/*
 * Pattern.cpp
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 */

#include "Pattern.h"
#include "PatternTagById.h"
#include "common/Misc.h"
#include "common/Maths.h"
#include "common/TimeKeeper.h"
#include <sstream>
#include <iostream>

namespace cryomesh {
namespace state {
// statics
int Pattern::ids = 1;

Pattern Pattern::getRandom(unsigned int width, double fraction) {
	std::vector<bool> ranvec;
	for (unsigned int i = 0; i < width; i++) {
		ranvec.push_back(common::Maths::getRandomBool(fraction));
	}
	assert(ranvec.size()==width);
	return Pattern(ranvec);
}
//END statics

Pattern::Pattern() {
	initialise();
}
Pattern::Pattern(const Pattern & pat) : Tagged(pat.getUUID()) {
	initialise();
	this->setId(pat.getId());
	binaryString = BinaryString(pat.getBinaryString());
}
Pattern::Pattern(const std::vector<bool> & pat) {
	initialise();
	binaryString = BinaryString(pat, false);
}
Pattern::Pattern(const std::string & str) {
	initialise();
	binaryString = BinaryString(str, false, BinaryString::BIN);
}
Pattern::~Pattern() {

}

Pattern& Pattern::operator=(const Pattern & obj) {
	if (&obj == this) {
		return *this;
	}
	initialise();
	this->setId(obj.getId());
	binaryString = BinaryString(obj.getBinaryString());
	return *this;
}
bool Pattern::isAllZeroes() const {
	return binaryString.isAllZeroes();
}
double Pattern::compare(const Pattern & obj) const {

	const std::vector<bool> this_pat = this->getPattern();
	const std::vector<bool> obj_pat = obj.getPattern();
	int this_sz = this_pat.size();
	int obj_sz = obj_pat.size();

#ifdef PATTERN_DEBUG // DEBUG
	std::cout << "Pattern::compare: this: ";
	common::Misc::print(std::cout, this_pat);
	std::cout << std::endl;
	std::cout << "Pattern::compare: obj: ";
	common::Misc::print(std::cout, obj_pat);
	std::cout << std::endl;
#endif // DEBUG
	if (this_sz == 0) {
		std::cout << "Pattern::compare: this zero pattern mismatch" << std::endl;
		return 0;
	} else {
		if (obj_sz == 0) {
			std::cout << "Pattern::compare: obj zero pattern mismatch" << std::endl;
			return 0;
		}
	}
	if (this_sz != obj_sz) {

		std::cout << "Pattern::compare: size mismatch " << std::endl;
		return 0;
	}
	double fraction = (double) 1 / (double) this_sz;
	double match = 0;

	std::vector<bool>::const_iterator it_this_vec = this_pat.begin();
	const std::vector<bool>::const_iterator it_this_vec_end = this_pat.end();
	std::vector<bool>::const_iterator it_obj_vec = obj_pat.begin();
	const std::vector<bool>::const_iterator it_obj_vec_end = obj_pat.end();

	while (it_this_vec != it_this_vec_end && it_obj_vec != it_obj_vec_end) {
		if (*it_this_vec == *it_obj_vec) {
			match += fraction;
		}
		++it_this_vec;
		++it_obj_vec;
	}
#ifdef PATTERN_DEBUG // DEBUG
	std::cout << "Pattern::compare: match: " << match << std::endl;
#endif // DEBUG
	return match;

}

// worker methods
bool Pattern::operator==(const Pattern & obj) const {
	bool match = true;
	if (&obj == 0) {
		std::cout << "Pattern::operator ==: obj is null" << std::endl;
		return false;
	}

	match = match && (this->getPattern() == obj.getPattern());
	if (match == false) {
		std::cout << "Pattern::equals: vector mismatch" << std::endl;
		return false;
	}
	return true;
}
bool Pattern::operator<(const Pattern & obj) const {
	return this->getId() < obj.getId();
}
// get/set
std::vector<bool> Pattern::getPattern() const {
	return binaryString.getBools();
}
std::string Pattern::getString() const {
	return Pattern::patternToString(this->getPattern());
}
void Pattern::setPattern(const std::vector<bool> & pat) {
	binaryString.setBinaryString(pat);
}
int Pattern::getId() const {
	return id;
}
void Pattern::setId(int new_id) {
#ifdef PATTERN_DEBUG
	std::cout << "Pattern::setId: setting id breaks uniqueness" << std::endl;
#endif // PATTERN_DEBUG
	id = new_id;
}
int Pattern::getWidth() const {
	return (this->getSize());
}

int Pattern::getSize() const {
	return (binaryString.getWidth());
}

const BinaryString & Pattern::getBinaryString() const {
	return binaryString;
}
BinaryString & Pattern::getMutableBinaryString() {
	return binaryString;
}

std::string Pattern::patternToString(const std::vector<bool> & vec) {
	if (vec.size() == 0) {
		return "";
	}
	std::stringstream ss;
	std::vector<bool>::const_iterator it_vec = vec.begin();
	const std::vector<bool>::const_iterator it_vec_end = vec.end();
	while (it_vec != it_vec_end) {
		if (*it_vec == 0) {
			ss << "0";
		} else {
			ss << "1";
		}
		++it_vec;
	}
	return ss.str();
}

std::vector<bool> Pattern::stringToPattern(const std::string & str) {
	std::vector<bool> vec_bool;
	if (str == "") {
		return vec_bool;
	}
	std::string::const_iterator it_str = str.begin();
	const std::string::const_iterator it_str_end = str.end();
	while (it_str != it_str_end) {
		if ((*it_str) == '1') {
			vec_bool.push_back(true);
		} else {
			if (*it_str == '0') {
				vec_bool.push_back(false);
			} else {
				return std::vector<bool>();
			}
		}
		++it_str;
	}
	return vec_bool;
}
const boost::shared_ptr<PatternTag> Pattern::getPatternTag() const {
	return patternTag;
}
void Pattern::setPatternTag(boost::shared_ptr<PatternTag> pt) {
	patternTag = pt;
}

int Pattern::getIds() {
	return Pattern::ids;
}
void Pattern::setIds(int is) {
#ifdef PATTERN_DEBUG
	std::cout << "Pattern::setIds: setting ids may break uniqueness" << std::endl;
#endif //PATTERN_DEBUG
	Pattern::ids = is;
}

boost::shared_ptr<manager::DatabaseObject> Pattern::getDatabaseObject() const {
	unsigned long int cycle = common::TimeKeeper::getTimeKeeper().getCycle().toULInt();

	boost::shared_ptr<manager::DatabaseObject> temp(
			new manager::PatternDatabaseObject(this->getUUIDString(), cycle, this->getString()));
	return temp;
}

void Pattern::initialise() {
	id = Pattern::getIds();
	++ids;

	//default pattern tag by id
	patternTag = boost::shared_ptr<PatternTag>(new PatternTagById(id));
}

std::ostream & operator<<(std::ostream& os, const Pattern & obj) {
	os << "Pattern: " << &obj << ": " << obj.getId() << ": " << obj.getString();
	return os;
}

}

}
