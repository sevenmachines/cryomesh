/**
 * Defs.h
 *
 *  Created on: 26 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace cryomesh {



/**
 * Pointer struct to allow typdef of templated smart pointers
 */
template<class T>
struct Pointer {
#ifdef CPP0X
typedef std::scoped_ptr<T> scoped_ptr;
typedef std::shared_ptr<T> shared_ptr;
#else
typedef boost::scoped_ptr<T> scoped_ptr;
typedef boost::shared_ptr<T> shared_ptr;
#endif
};

}

#endif /* DEFS_H_ */
