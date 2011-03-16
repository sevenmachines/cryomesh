/**
 * Cycle.h
 *
 *  Created on: 1 Feb 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef CYCLE_H_
#define CYCLE_H_

#include <gmpxx.h>

/**
 *  Wrapper class around implementation of a cycle
 */
namespace cryomesh {

namespace common {

class Cycle {
public:

	/**
	 * Default Constructor
	 */
	Cycle();

	/**
	 * Construct from unsigned long int
	 *
	 * @param const unsigned long int
	 * 		The ulong int to contruct from
	 */
	Cycle(const  long int it);

	/**
	 * Construct from mpz
	 *
	 * @param mpz_class mp
	 * 		The multi-precision integer to contruct from
	 */
	Cycle(mpz_class mp);

	/**
	 * Assignment operator
	 *
	 * @param const Cycle & obj
	 * 		RHS assignment
	 *
	 * @return Cycle &
	 * 		This object after assignment
	 */

	Cycle & operator=(const Cycle & obj);

	/**
	 * Non-destructive addition operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return Cycle
	 * 		New object after addition
	 */
	const Cycle operator+(const Cycle & obj) const;

	/**
	 * Greater than operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return bool
	 * 		True if > than obj, false otherwise
	 */
	bool operator>(const Cycle & obj) const;

	/**
	 * Less than operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return bool
	 * 		True if less than obj, false otherwise
	 */
	bool operator<(const Cycle & obj) const;

	/**
	 * >= than operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return bool
	 * 		True if > than obj, false otherwise
	 */
	bool operator>=(const Cycle & obj) const;

	/**
	 * <= than operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return bool
	 * 		True if less than obj, false otherwise
	 */
	bool operator<=(const Cycle & obj) const;

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return Cycle &
	 * 		This object after addition and assignment
	 */
	Cycle & operator+=(const Cycle & obj);

	/**
	 * Non-destructive addition operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return Cycle
	 * 		New object after addition
	 */
	const Cycle operator-(const Cycle & obj) const;

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const Cycle & obj
	 * 		RHS addition
	 *
	 * @return Cycle &
	 * 		This object after addition and assignment
	 */
	Cycle & operator-=(const Cycle & obj);

	/**
	 * Comparator operator
	 *
	 * @param const Cycle & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	bool operator==(const Cycle & obj) const;

	/**
	 * Not comparator operator
	 *
	 * @param const Cycle & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if not equal, false otherwise
	 */
	bool operator!=(const Cycle &obj) const;

	/**
	 * Prefix increment operator
	 *
	 * @return Cycle &
	 * 		Return this
	 */
	Cycle & operator++();

	/**
	 * Postfix increment operator
	 *
	 * @return Cycle &
	 * 		Return this
	 */
	Cycle operator++(int);

	/**
	 * Postfix decrement operator
	 *
	 * @return Cycle &
	 * 		Return this
	 */
	Cycle operator--(int);

	/**
	 * Prefix increment operator
	 *
	 * @return Cycle &
	 * 		Return this
	 */
	Cycle & operator--();

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Cycle & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Cycle & obj);

	/**
	 * Return as an unsigned integer
	 *
	 * @return unsigned int
	 * 		The cycle as an int
	 */
	unsigned long int toULInt() const;

	/**
	 * Return as an signed integer
	 *
	 * @return signed int
	 * 		The cycle as an int
	 */
	long int toLInt() const;

	/**
	 *  Get multiprecision value
	 *
	 *  @return mpz_class
	 */
	const mpz_class & getMP() const;

private:
	/**
	 * The current cycle
	 *
	 * @var mpz_class
	 */
	mpz_class cycle;
};

}//NAMESPACE

}//NAMESPACE

#endif /* CYCLE_H_ */
