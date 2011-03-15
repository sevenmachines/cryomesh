/**
 * ICollection.h
 *
 *  Created on: 1 Feb 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef ICOLLECTION_H_
#define ICOLLECTION_H_

/**
 *  Interface for collection classes
 */
namespace cryomesh {

namespace common {

template<class T>
class ICollection {
public:
	virtual ~ICollection() {
	}

	/**
	 * Not comparator operator
	 *
	 * @param const T & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if not equal, false otherwise
	 */
	virtual bool operator!=(const T &obj) const = 0;

	/**
	 * Non-destructive addition operator
	 *
	 * @param const T & obj
	 * 		RHS addition
	 *
	 * @return T
	 * 		New object after addition
	 */
	virtual const T operator+(const T & obj) const = 0;

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const T & obj
	 * 		RHS addition
	 *
	 * @return T &
	 * 		This object after addition and assignment
	 */
	virtual T & operator+=(const T & obj) = 0;

	/**
	 * Assignment operator
	 *
	 * @param const T & obj
	 * 		RHS assignment
	 *
	 * @return T &
	 * 		This object after assignment
	 */
	virtual T & operator=(const T & obj) = 0;

	/**
	 * Comparator operator
	 *
	 * @param const T & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	virtual bool operator==(const T & obj) const = 0;

protected:
	ICollection() {
	}
};

}

}

#endif /* ICOLLECTION_H_ */
