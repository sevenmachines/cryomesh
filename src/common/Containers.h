/*
 * Containers.h
 *
 *  Created on: 20 Sep 2010
 *      Author: SevenMachines@yahoo.co.uk
 *
 *      Set of common operations on containers
 *
 */
#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <list>

#ifndef CONTAINERS_H_
#define CONTAINERS_H_

namespace cryomesh {

namespace common {

/**
 * Containers contains utility functions for containers
 */
class Containers {
public:
	/**
	 * Destructor
	 */
	virtual ~Containers();

	/**
	 * delete entries by comparison
	 *
	 * @param U
	 * 	Comparison value
	 * @param std::multimap<U, T>
	 * 	MultiMap to use
	 *@param int
	 *		Comparison to use, result<0 for less than, 0 for equal, >0 for greater than
	 *
	 * @return std::multimap<U, T>
	 * 	Modified map
	 */
	template<class U, class T>
	static std::multimap<U, T> deleteByComparison(const U & val, std::multimap<U, T> & mapobj, int result) {

		// forall in mapobj
		{
			typename std::multimap<U, T>::iterator it_mapobj = mapobj.begin();
			const typename std::multimap<U, T>::const_iterator it_mapobj_end = mapobj.end();
			while (it_mapobj != mapobj.end()) {
				if (result < 0 && it_mapobj->first < val) {
					std::cout<<"Containers::deleteByComparison: "<<"Deleting "<< it_mapobj->first<<" ( < "<<val<<")"<<std::endl;
					mapobj.erase(it_mapobj->first);
				} else if (result == 0 && it_mapobj->first == val) {
					mapobj.erase(it_mapobj->first);
				} else if (result > 0 && it_mapobj->first > val) {
					mapobj.erase(it_mapobj->first);
				}
				++it_mapobj;
			}
		}
		return mapobj;

	}
	template<class U, class T>
	static std::map<boost::shared_ptr<U>, boost::shared_ptr<T> > addMapObjectByCreationObjects(
			std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj, std::list<boost::shared_ptr<U> > & colist) {
		typename std::map<boost::shared_ptr<U>, boost::shared_ptr<T> > new_objs;
		//for all in colist
		{
			typename std::list<boost::shared_ptr<U> >::const_iterator it_colist = colist.begin();
			const typename std::list<boost::shared_ptr<U> >::const_iterator it_colist_end = colist.end();
			while (it_colist != it_colist_end) {
				boost::shared_ptr<T> newobj = (*it_colist)->create();
				mapobj[newobj->getUUID()] = (newobj);
				new_objs[*it_colist] = newobj;
				++it_colist;
			}
		}

		return new_objs;
	}

	template<class U, class T>
	static bool deleteByEntry(U & store, const T & id) {
		bool found = false;
		typename U::iterator it = std::find(store.begin(), store.end(), id);
		if (it != store.end()) {
			store.erase(it);
			found = true;

		}
		return found;

	}
	template<class T>
	static boost::shared_ptr<T> deleteMapObjectByUUID(std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj,
			const boost::uuids::uuid & id) {
		boost::shared_ptr<T> temp_obj = common::Containers::getMutableByUUID<T>(mapobj, id);
		if (temp_obj != 0) {
			temp_obj->isolate();
			mapobj.erase(id);
		} else {
			std::cout << "Containers::deleteMapObjectByUUID: " << "WARNING: uuid not found in map object " << std::endl;
		}

		return temp_obj;
	}

	template<class T>
	static const boost::shared_ptr<T> getByUUID(const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj,
			const boost::uuids::uuid id) {
		boost::shared_ptr<T> temp_obj;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_found = mapobj.find(id);
		const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_end = mapobj.end();

		if (it_found == it_end) {
			std::cout << "Containers::getByUUID: " << "WARNING: UUID not found " << std::endl;
		} else {
			temp_obj = it_found->second;
		}
		return temp_obj;
	}

	template<class T>
	static boost::shared_ptr<T> getMutableByUUID(std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj,
			boost::uuids::uuid id) {
		boost::shared_ptr<T> temp_obj;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_found = mapobj.find(id);
		const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_end = mapobj.end();

		if (it_found == it_end) {
			std::cout << "Containers::getByUUID: " << "WARNING: UUID not found " << std::endl;
		} else {
			temp_obj = it_found->second;
		}
		return temp_obj;
	}

	template<class T>
	static const boost::shared_ptr<T> getByID(const std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj,
			int id) {

		boost::shared_ptr<T> temp_obj;

		bool found = false;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_map = mapobj.begin();
		const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_end = mapobj.end();
		while (it_map != it_end && found == false) {
			if (it_map->second->getId() == id) {
				temp_obj = *(it_map->second);
				found = true;
			}
			++it_map;
		}
		if (it_map == it_end) {
			std::cout << "Containers::getByID: " << "WARNING: ID not found " << std::endl;
		} else {
			temp_obj = it_map->second;
		}
		return temp_obj;
	}

	template<class T>
	static boost::shared_ptr<T> getMutableByID(std::map<boost::uuids::uuid, boost::shared_ptr<T> > & mapobj, int id) {
		boost::shared_ptr<T> temp_obj;

		bool found = false;
		typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_map = mapobj.begin();
		const typename std::map<boost::uuids::uuid, boost::shared_ptr<T> >::const_iterator it_end = mapobj.end();
		while (it_map != it_end && found == false) {
			if (it_map->second->getId() == id) {
				temp_obj = *(it_map->second);
				found = true;
			}
			++it_map;
		}
		if (it_map == it_end) {
			std::cout << "Containers::getByID: " << "WARNING: ID not found " << std::endl;
		} else {
			temp_obj = it_map->second;
		}
		return temp_obj;
	}

	template<class U, class T>
	static void mapByUUID(const std::map<boost::shared_ptr<U>, boost::shared_ptr<T> > & creation_to_real,
			std::map<boost::uuids::uuid, boost::uuids::uuid> & uuid_map) {
		// forall in new_chans
		{
			typename std::map<boost::shared_ptr<U>, boost::shared_ptr<T> >::const_iterator it_objs =
					creation_to_real.begin();
			const typename std::map<boost::shared_ptr<U>, boost::shared_ptr<T> >::const_iterator it_objs_end =
					creation_to_real.end();
			while (it_objs != it_objs_end) {
				uuid_map[it_objs->first->getUUID()] = it_objs->second->getUUID();
				++it_objs;
			}
		}
	}

	/**
	 * Print a mapped collection to an output stream
	 *
	 * @param std::ostream & os
	 * 		Stream to print to
	 * @param const T & obj
	 * 		object to print out
	 *
	 * @return std::ostream &
	 * 		The output stream
	 */
	template<class U, class T>
	static std::ostream & print(std::ostream & os, const std::map<U, T> & obj) {
		os << "Object:" << &obj << " { ";
		// forall in obj
		{
			typename std::map<U, T>::const_iterator it_obj = obj.begin();
			const typename std::map<U, T>::const_iterator it_obj_end = obj.end();
			while (it_obj != it_obj_end) {
				os << it_obj->first << ": " << it_obj->secong << " ";
				++it_obj;
			}
			os << "}";
		}
		return os;
	}

	/**
	 * Print a collection to an output stream
	 *
	 * @param std::ostream & os
	 * 		Stream to print to
	 * @param const T & obj
	 * 		object to print out
	 *
	 * @return std::ostream &
	 * 		The output stream
	 */
	template<class T>
	static std::ostream & print(std::ostream & os, const T & obj) {
		os << "Object:" << &obj << " { ";
		// forall in obj
		{
			typename T::const_iterator it_obj = obj.begin();
			const typename T::const_iterator it_obj_end = obj.end();
			while (it_obj != it_obj_end) {
				os << *it_obj << " ";
				++it_obj;
			}
			os << "}";
		}
		return os;
	}
	/**
	 * Compare two collections
	 *
	 * @param const T & lhs
	 * 		First object
	 * @param const T & rhs
	 * 		Second object
	 *
	 * @return bool
	 * 		True of equal, false otherwise
	 */
	template<class T>
	static bool compare(const T & lhs, const T & rhs) {
		if (lhs.size() != rhs.size()) {
			return false;
		}
		// forall in obj1,2
		{
			typename T::const_iterator it_lhs = lhs.begin();
			const typename T::const_iterator it_lhs_end = lhs.end();
			typename T::const_iterator it_rhs = rhs.begin();
			const typename T::const_iterator it_rhs_end = rhs.end();
			while (it_lhs != it_lhs_end && it_rhs != it_rhs_end) {
				//std::cout << "Containers::compare: " << *it_lhs << "==" << *it_rhs << std::endl;
				if (*it_lhs != *it_rhs) {
					return false;
				}
				++it_lhs;
				++it_rhs;
			}
		}
		return true;
	}
	/**
	 * Template function to add a collection, rhs, in-place to a collection on lhs
	 *
	 * @param T & lhs
	 * 		Left hand side parameter which will be destructively added into
	 * @param const T & rhs
	 * 		Right hand side which will be added but left unchanged
	 *
	 * @ return T &
	 * 		The merged collection
	 */
	template<class T>
	static T & addMerge(T & lhs, const T & rhs) {
		// resize if needed
		//std::cout << "Containers::addMerge: " << "" << std::endl;
		//std::cout<<Containers::print(std::cout, lhs)<<std::endl;
		//std::cout<<Containers::print(std::cout, rhs)<<std::endl;
		int size_diff = rhs.size() - lhs.size();
		if (size_diff > 0) {
			lhs.resize(lhs.size() + size_diff, 0);
		}
		// forall in lhs
		{
			typename T::iterator it_lhs = lhs.begin();
			const typename T::const_iterator it_lhs_end = lhs.end();
			typename T::const_iterator it_rhs = rhs.begin();
			const typename T::const_iterator it_rhs_end = rhs.end();
			while (it_rhs != it_rhs_end) {
				//std::cout<<"Containers::addMerge: "<<*it_lhs<<"+"<<*it_rhs;
				*it_lhs += *it_rhs;
				//std::cout<<"="<<*it_lhs<<std::endl;
				++it_lhs;
				++it_rhs;
			}
		}
		return lhs;
	}

	/**
	 * Template function to add a collection, rhs, non destructively to a collection on lhs
	 *
	 * @param T & lhs
	 * 		Left hand side parameter which will be added and left unchanged
	 * @param const T & rhs
	 * 		Right hand side which will be added and left unchanged
	 *
	 * @ return T
	 * 		The merged collection
	 */
	template<class T>
	static T add(const T & lhs, const T & rhs) {
		T newcoll;
		// forall in rhs
		{
			typename T::const_iterator it_rhs = rhs.begin();
			const typename T::const_iterator it_rhs_end = rhs.end();
			while (it_rhs != it_rhs_end) {
				newcoll.push_back(*it_rhs);
				++it_rhs;
			}
		}

		Containers::addMerge(newcoll, lhs);
		return newcoll;
	}
protected:

	/**
	 * Protected constructor to prevent instantiation
	 */
	Containers();
};

}

}

#endif /* CONTAINERS_H_ */
