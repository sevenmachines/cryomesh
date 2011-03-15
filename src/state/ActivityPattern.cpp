/*
 * ActivityPattern.cpp
 *
 *  Created on: 23 Feb 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "ActivityPattern.h"
#include <list>
#include "common/Maths.h"
#include "common/Misc.h"
#include <sstream>

namespace cryomesh {

namespace state {

ActivityPattern::ActivityPattern() {

}

ActivityPattern::~ActivityPattern() {
}

std::string ActivityPattern::toPlusBooleanString() const {
	std::stringstream ss;
	common::Misc::print(ss, this->toPlusBooleanList());
	return ss.str();
}

std::list<bool> ActivityPattern::toPlusBooleanList() const {
	return this->toBooleanVector(0, GreaterThan);
}

std::list<bool> ActivityPattern::toBooleanVector(const double cuttoff, const ActivityComparison compare) const{
	std::list<bool> compare_result;
	const std::list<double> & all_doubles = this->getCollection();
	// forall in all_doubles
	{
		std::list<double>::const_iterator it_all_doubles = all_doubles.begin();
		const std::list<double>::const_iterator it_all_doubles_end = all_doubles.end();
		while (it_all_doubles != it_all_doubles_end) {
			if (compare == GreaterThan) {
				if (*it_all_doubles > cuttoff) {
					compare_result.push_back(true);
				} else {
					compare_result.push_back(false);
				}
			} else if (compare == LessThan) {
				if (*it_all_doubles < cuttoff) {
					compare_result.push_back(true);
				} else {
					compare_result.push_back(false);
				}
			} else if (compare == EqualTo) {
				if (common::Maths::compareDoubles(*it_all_doubles, cuttoff) == true) {
					compare_result.push_back(true);
				} else {
					compare_result.push_back(false);
				}
			}
			++it_all_doubles;
		}
	}
	return compare_result;
}
}//NAMESPACE

}//NAMESPACE
