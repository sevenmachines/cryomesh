/*
 * ActivityPattern.h
 *
 *  Created on: 23 Feb 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef ACTIVITYPATTERN_H_
#define ACTIVITYPATTERN_H_

#include "common/SimpleCollection.h"

namespace cryomesh {

namespace state {

/**
 * A simple collection of doubles representing a pattern of activities
 */
class ActivityPattern: public common::SimpleCollection<double> {
public:
	enum ActivityComparison{
		GreaterThan, LessThan, EqualTo
	};
	ActivityPattern();
	virtual ~ActivityPattern();

	/**
	 * Return a string of booleans representing with each element is > 0 or not
	 *
	 * @return std::string
	 * 		The string of booleans
	 */
	virtual std::string toPlusBooleanString() const;


	/**
	 * Return a vector of booleans representing with each element  > 0 or not
	 *
	 * @return std::vector<bool>
	 * 		The vector of booleans
	 */
	virtual std::list<bool> toPlusBooleanList() const;

protected:
	std::list<bool> toBooleanVector(const double cuttoff, const ActivityComparison compare)const ;

};

}

}

#endif /* ACTIVITYPATTERN_H_ */
