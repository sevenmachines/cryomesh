/*
 * KeyStore.h
 *
 *  Created on: 13 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef KEYSTORE_H_
#define KEYSTORE_H_

#include <map>

namespace cryomesh {

namespace common {

/**
 * Simple class to store and retrieve keys
 */
template<class U, class T>
class KeyStore {
public:

	/**
	 * Default constructor
	 */
	KeyStore() {
		nullPair.insert(std::pair<U, T>(U(), T()));
		it_null = nullPair.begin();
		objects.clear();
	}

	/**
	 * Default destructor
	 */
	virtual ~KeyStore() {
	}

	/**
	 * Add an initialiser list of key/object pairs
	 *
	 * @param std::initializer_list<std::pair<U, T> >
	 * 	Initialiser list of key/object pairs
	 */
	void add(const std::initializer_list<std::pair<U, T> > & list) {
		// forall in list
		{
			typename std::initializer_list<std::pair<U, T> >::iterator it_list = list.begin();
			const typename std::initializer_list<std::pair<U, T> >::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				objects.insert(*it_list);
				++it_list;
			}
		}
	}

	/**
	 * Add a key/object pairs
	 *
	 * @param std::pair<U, T>
	 * 	 key/object pairs
	 */
	void add(const U & first, const T & second) {
		this->add( { std::pair<U, T>(first, second) });
	}

	/**
	 * Remove objects from collection by initialiser list
	 *
	 * @param  std::initializer_list<U > obj
	 * 		The values to be removed from the collection
	 */
	virtual void remove(const std::initializer_list<U> & list) {
		// forall in list
		{
			typename std::initializer_list<U>::iterator it_list = list.begin();
			const typename std::initializer_list<U>::const_iterator it_list_end = list.end();
			while (it_list != it_list_end) {
				this->remove(*it_list);
				++it_list;
			}
		}
	}

	/**
	 * Remove object from collection bt key
	 *
	 * @param U key
	 * 		The key of the object to be removed from the collection
	 *
	 * @return boost::shared_ptr<U>
	 * 		The object removed from the collection
	 *
	 */
	void remove(U key) {
		typename std::map<U, T>::iterator it_found = objects.find(key);

		if (it_found != objects.end()) {
			objects.erase(it_found);
		}
	}

	/**
	 * Get iterator to key or null iterator otherwise
	 *
	 * @param U
	 * 	Key to search for
	 */
	virtual const typename std::map<U, T>::iterator get(const U & key) {
		typename std::map<U, T>::iterator it_temp = this->getNullEntry();
		typename std::map<U, T>::iterator it_found = objects.find(key);
		if (it_found != objects.end()) {
			it_temp = it_found;
		}
		return it_temp;
	}

	/**
	 * Get iteratorto null
	 *
	 */
	virtual const typename std::map<U, T>::iterator getNullEntry() {
		typename std::map<U, T>::iterator it_temp = it_null;
		return it_temp;
	}
	/**
	 * Clear all from store
	 */
	virtual void clear() {
		objects.clear();
	}

	/**
	 * Return size
	 *
	 * @ return size_t
	 * 	Size of stored items
	 */
	virtual size_t size() const {
		return objects.size();
	}

protected:
	/**
	 * Default null value entry
	 *
	 * @ var std::map<U, T>
	 */
	std::map<U, T> nullPair;

	/**
	 * Iterator to the deault null
	 *
	 * @var std::map<U, T>
	 */
	typename std::map<U, T>::iterator it_null;

private:
	std::map<U, T> objects;
};

}

}

#endif /* KEYSTORE_H_ */
