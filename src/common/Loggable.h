/*
 * Loggable.h
 *
 *  Created on: 13 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef LOGGABLE_H_
#define LOGGABLE_H_

#include <iostream>

namespace cryomesh {

namespace common {

class Loggable {
public:
	/**
		 * Enum representing print detail
		 */

	enum LoggingDepth{
		SUMMARY, MAX
	};
	Loggable(){}
	virtual ~Loggable(){}

	std::ostream & print(std::ostream & os, const Loggable::LoggingDepth depth = Loggable::SUMMARY)const{return os;}
};

}

}

#endif /* LOGGABLE_H_ */
