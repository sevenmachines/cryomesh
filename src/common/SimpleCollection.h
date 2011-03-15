/**
 * SimpleCollection.h
 *
 *  Created on: 27 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef SIMPLECOLLECTION_H_
#define SIMPLECOLLECTION_H_

#include "common/exceptions/IndexOutOfBoundsException.h"
#include "common/Containers.h"
#include "common/ICollection.h"

#include <list>
#include <iostream>
#include <algorithm>
#include <ostream>

namespace cryomesh {

namespace common {

/**
 *  SimpleCollection of objects with accessors
 */
template<class T>
class SimpleCollection: public ICollection<SimpleCollection<T> > {
public:

	/**
	 * Default constructor
	 */
	SimpleCollection() {
	}

	/**
	 * Construct from initializer list
	 *
	 * @param std::initialiser_list<T> obj
	 * 		The values to be added to the collection
	 */
	SimpleCollection(const std::initializer_list<T> & list) {
		this->add(list);
	}

	/**
	 * Construct by list
	 *
	 * @param std::list<T> list
	 * 		The list values to be added to the collection
	 */
	SimpleCollection(const std::list<T> & list) {
		this->add(list);
	}

	/**
	 * Default destructor
	 */
	virtual ~SimpleCollection() {
	}

	/**
	 * Clear all objects from collection
	 */
	virtual void clear() {
		objects.clear();
	}

	/**
	 * Add object to collection
	 *
	 * @param T obj
	 * 		The value to be added to the collection
	 *
	 * @return T
	 * 		The value added to the collection
	 *
	 */
	virtual void add(T obj) {
		objects.push_back(obj);
	}

	/**
	 * Add object to collection by initialiser list
	 *
	 * @param std::initialiser_list<T> obj
	 * 		The values to be added to the collection
	 */
	virtual void add(const std::initializer_list<T> & list) {
		// forall in list
		{
			typename std::initializer_list<T>::iterator it_list = list.begin();
			const typename std::initializer_list<T>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				this->add(*it_list);
				++it_list;
			}
		}
	}

	/**
	 * Add object to collection by list
	 *
	 * @param std::list<T> list
	 * 		The list values to be added to the collection
	 */
	virtual void add(const std::list<T> & list) {
		// forall in list
		{
			typename std::list<T>::const_iterator it_list = list.begin();
			const typename std::list<T>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				this->add(*it_list);
				++it_list;
			}
		}
	}

	/**
	 * Remove object from collection
	 *
	 * @param T obj
	 * 		The object to be removed from the collection
	 */
	virtual void remove(T obj) {
		// forall in objects
		{
			typename std::list<T>::iterator it_objects = objects.begin();
			typename std::list<T>::const_iterator it_objects_end = objects.end();

			while (it_objects != it_objects_end) {
				if (*it_objects == obj) {
					objects.erase(it_objects);
					it_objects = objects.begin();
					it_objects_end = objects.end();
				}
				++it_objects;
			}
		}
	}

	/**
	 * Remove objects from collection by initialiser list
	 *
	 * @param std::initialiser_list<T> obj
	 * 		The values to be removed from the collection
	 */
	virtual void remove(const std::initializer_list<T> & list) {
		// forall in list
		{
			typename std::initializer_list<T>::iterator it_list = list.begin();
			const typename std::initializer_list<T>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				this->remove(*it_list);
				++it_list;
			}
		}
	}

	/**
	 * Remove objects from collection by list
	 *
	 * @param std::list<T> list
	 * 		The list values to be removed to the collection
	 */
	virtual void remove(const std::list<T> & list) {
		// forall in list
		{
			typename std::list<T>::const_iterator it_list = list.begin();
			const typename std::list<T>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				this->remove(*it_list);
				++it_list;
			}
		}
	}

	/*
	 * Calculate the sum of all elements
	 *
	 * return T
	 * 		Sum of all elements
	 */
	virtual T getSum() const {
		T total(0);
		// forall in objects
		{
			typename std::list<T>::const_iterator it_objects = objects.begin();
			const typename std::list<T>::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end) {
				total += *it_objects;
				++it_objects;
			}
		}
		return total;
	}

	/*
	 * Get max value
	 *
	 * @return  T
	 * 		The maximum value
	 */
	virtual T getMaximum() const {
		return this->getBoundaryValue(true);
	}

	/*
	 * Get min value
	 *
	 * @return  T
	 * 		The minimum value
	 */
	virtual T getMinimum() const {
		return this->getBoundaryValue(false);
	}

	/**
	 * Get value at index
	 *
	 * @param unsigned index
	 * 		Index to return
	 *
	 * @return T
	 * 		Value at index, 0 if out of bounds
	 */
	virtual T operator[](unsigned index) const throw (exceptions::IndexOutOfBoundsException) {

		typename std::list<T>::const_iterator it_value;

		//Check for out of bounds
		if (index > this->getSize()) {
			throw exceptions::IndexOutOfBoundsException();
		}

		// forall in objects, find value at index
		{
			bool found = false;
			int count = 0;

			typename std::list<T>::const_iterator it_objects = objects.begin();
			const typename std::list<T>::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end && found == false) {
				if (count == index) {
					it_value = it_objects;
					found = true;
				}
				++count;
				++it_objects;
			}
		}
		return *it_value;
	}

	/**
	 * Invert collection
	 *
	 * 		Reflect the collection in the x axis
	 *
	 * @return SimpleCollection &
	 * 		The inverted object
	 */
	virtual SimpleCollection & invert() {
		// forall in objects
		{
			typename std::list<T>::iterator it_objects = objects.begin();
			const typename std::list<T>::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end) {
				*it_objects = -*it_objects;
				++it_objects;
			}
		}
		return *this;
	}

	/**
	 *	Reverse collection
	 *
	 *		Reflect the collection in the y axis
	 *
	 * @return SimpleCollection<T> &
	 * 		The inverted object
	 */
	virtual SimpleCollection<T> & reverse() {
		std::reverse(objects.begin(), objects.end());
		return *this;
	}

	// GETTERS/SETTERS

	/**
	 * Get underlying collection
	 *
	 * @return const std::list<T> &
	 * 		The collection
	 */
	virtual const std::list<T> & getCollection() const {
		return objects;
	}

	/**
	 * Get mutable underlying collection
	 *
	 * @return std::list<T> &
	 * 		The mutable collection
	 */
	virtual std::list<T> & getMutableCollection() {
		return objects;
	}

	/**
	 * Get underlying collections size
	 *
	 * @return int
	 * 		The collections size
	 */
	virtual int getSize() const {
		return objects.size();
	}

	/**
	 * Non-destructive addition operator
	 *
	 * @param const SimpleCollection<T> & obj
	 * 		RHS addition
	 *
	 * @return SimpleCollection<T>
	 * 		New object after addition
	 */
	virtual const SimpleCollection<T> operator+(const SimpleCollection<T> & obj) const {
		SimpleCollection<T> newobj = *this;
		newobj += obj;
		return newobj;
	}

	/**
	 * Destructive addition and assignment operator
	 *
	 * @param const SimpleCollection<T> & obj
	 * 		RHS addition
	 *
	 * @return SimpleCollection<T> &
	 * 		This object after addition and assignment
	 */
	virtual SimpleCollection<T> & operator+=(const SimpleCollection<T> & obj) {
		const std::list<T> & all_objs = obj.getCollection();
		std::list<T> & this_objs = this->getMutableCollection();
		common::Containers::addMerge<std::list<T> >(this_objs, all_objs);
		return *this;
	}

	/**
	 * Assignment operator
	 *
	 * @param const SimpleCollection<T> & obj
	 * 		RHS assignment
	 *
	 * @return SimpleCollection<T> &
	 * 		This object after assignment
	 */
	virtual SimpleCollection<T> & operator=(const SimpleCollection<T> & obj) {
		if (*this == obj) {
			return *this;
		}

		this->objects.clear();
		const std::list<T> & all_objs = obj.getCollection();
		// forall in all_objs
		{
			typename std::list<T>::const_iterator it_all_objs = all_objs.begin();
			const typename std::list<T>::const_iterator it_all_objs_end = all_objs.end();
			while (it_all_objs != it_all_objs_end) {
				this->objects.push_back(*it_all_objs);
				++it_all_objs;
			}
		}
		return *this;
	}

	/**
	 * Comparator operator
	 *
	 * @param const SimpleCollection<T> & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if equal, false otherwise
	 */
	virtual bool operator==(const SimpleCollection<T> & obj) const {
		const std::list<T> & all_objs = obj.getCollection();
		const std::list<T> & this_objs = this->getCollection();

		if (this_objs.size() != all_objs.size()) {
			return false;
		}
#ifdef SIMPLECOLLECTION_DEBUG
		std::cout << "SimpleCollection::operator ==: " << "" << std::endl;
		std::cout << common::Containers::print(std::cout, this_objs) << std::endl;
		std::cout << common::Containers::print(std::cout, all_objs) << std::endl;
#endif
		// forall in all_objs
		{
			typename std::list<T>::const_iterator it_all_objs = all_objs.begin();
			const typename std::list<T>::const_iterator it_all_objs_end = all_objs.end();
			typename std::list<T>::const_iterator it_this_objs = this_objs.begin();
			const typename std::list<T>::const_iterator it_this_objs_end = this_objs.end();

			while (it_all_objs != it_all_objs_end && it_this_objs != it_this_objs_end) {
#ifdef SIMPLECOLLECTION_DEBUG
				std::cout << "SimpleCollection::operator==: " << *it_this_objs << "==" << *it_all_objs << std::endl;
#endif
				if (*it_all_objs != *it_this_objs) {
					return false;
				}
				++it_this_objs;
				++it_all_objs;
			}
		}
		return true;
	}

	/**
	 * Not comparator operator
	 *
	 * @param const SimpleCollection & obj
	 * 		RHS object
	 *
	 * @return bool
	 * 		True if not equal, false otherwise
	 */
	bool operator!=(const SimpleCollection &obj) const {
		return !(*this == obj);
	}

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & out
	 *		The output stream
	 *	@param const SimpleCollection & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & out, const SimpleCollection & obj) {
		out << "SimpleCollection: " << &obj << " objects: { ";
		// forall in objects
		{
			typename std::list<T>::const_iterator it_objects = obj.getCollection().begin();
			const typename std::list<T>::const_iterator it_objects_end = obj.getCollection().end();
			while (it_objects != it_objects_end) {
				out << *it_objects << " ";
				++it_objects;
			}
		}
		out << "}";
		return out;
	}

	/**
	 * Pad Collection
	 *
	 * @param int front_pad
	 * 		Amount of places to pad to front
	 * @param int back_pad
	 * 		Amount of places to pad to back
	 * @param T & obj
	 * 		Object to use for pad
	 *
	 * @return T &
	 * 		Return the collection
	 */
	std::list<T> & pad(int front_pad, int back_pad, T pad_value) {
		// check padding is >= 0
		front_pad = std::max(front_pad, 0);
		back_pad = std::max(back_pad, 0);

		for (int i = 0; i < front_pad; i++) {
			objects.push_front(pad_value);
		}
		for (int i = 0; i < back_pad; i++) {
			objects.push_back(pad_value);
		}
		return objects;
	}

protected:

	/*
	 * Get boundary values of the collection
	 *
	 * @param bool greater
	 * 		True if maximum boundary false if minimum
	 *
	 * @return const T &
	 * 		The boundary value
	 */
	T getBoundaryValue(bool greater) const {
		typename std::list<T>::const_iterator max = objects.begin();
		typename std::list<T>::const_iterator min = objects.begin();
		typename std::list<T>::const_iterator return_value;
		// forall in objects
		{
			typename std::list<T>::const_iterator it_objects = objects.begin();
			const typename std::list<T>::const_iterator it_objects_end = objects.end();
			while (it_objects != it_objects_end) {
				if (*it_objects > *max) {
					max = it_objects;
				}
				if (*it_objects < *min) {
					min = it_objects;
				}
				++it_objects;
			}
		}
		if (greater == true) {
			return_value = max;
		} else if (greater == false) {
			return_value = min;
		}
		if (return_value == objects.end()) {
			return T();
		}
		return *return_value;
	}

	/**
	 * Generate a curve from boundary value
	 *
	 *	@param const int max_y
	 * 		Boundary value of curve
	 * 	@param const int length
	 * 		Length of Impulse
	 */
	void generateCurve(const double max_y, const unsigned long length) {

		objects.clear();
		double max_x = static_cast<double> ((2 + length) / 2);
		for (unsigned long i = 0; i < length; ++i) {
			double newx = ((i + 1) / (max_x + 1)) - 1.0;
			double val = max_y * (1 - (newx * newx));
			//std::cout << "generateCurve: " << "length:" << length << "i:" << i << " newx:" << newx << " val:" << val
			//		<< std::endl;
			objects.push_back(val);
		}
	}

private:
	std::list<T> objects;
};

}

}

#endif /* SIMPLECOLLECTION_H_ */
