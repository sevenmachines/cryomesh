/**
 * IndexOutOfBoundsException.h
 *
 *  Created on: 28 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef INDEXOUTOFBOUNDSEXCEPTION_H_
#define INDEXOUTOFBOUNDSEXCEPTION_H_

#include <exception>

namespace cryomesh {

namespace common {

namespace exceptions {

/**
 *  IndexOutOfBoundsException is the exception thrown when accessing beyond the bounds of a collection
 */
class IndexOutOfBoundsException : public std::exception {
public:
	/**
	 * Overidden version of exception method
	 *
	 * @return const char *
	 * 		A description of the exception
	 */
	virtual const char* what() const throw() {
	    return "Index out of bounds exception.";
	  }
};

}

}

}

#endif /* INDEXOUTOFBOUNDSEXCEPTION_H_ */
