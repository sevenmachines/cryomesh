/**
 * StepMappedCollection.h
 *
 *  Created on: 9 Feb 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef STEPMAPPEDCOLLECTION_H_
#define STEPMAPPEDCOLLECTION_H_

#include <map>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace common {

/**
 *  A keymapped collection where the key represents some sort of
 *  stepped value
 */
template<class T, class U>
class StepMappedCollection {
public:
	StepMappedCollection() {
	}
	virtual ~StepMappedCollection() {
	}

	/**
	 * Update the collection one step
	 */
	void update() {
		U zeroed;
		++zeroed;
		this->update(zeroed);
	}

	/**
	 * Update the collection by steps
	 */
	void update(const U & steps) {
		while (activeRange.first < steps) {
			++activeRange.first;
			++activeRange.second;
		}
		// drop all historical objects
		clearRange(activeRange.second);
	}

	/**
	 * Get the stepped map
	 *
	 * @return const std::multimap<U, boost::shared_ptr<T> > &
	 * 		The stepped map
	 */
	const std::multimap<U, boost::shared_ptr<T> > & getSteppedMap() const {
		return steppedMap;
	}

	/**
	 * Get the mutable stepped map
	 *
	 * @return  std::multimap<U, boost::shared_ptr<T> > &
	 * 		The stepped map
	 */
	std::multimap<U, boost::shared_ptr<T> > & getMutableSteppedMap() {
		return steppedMap;
	}

	/**
	 * Get the collection range
	 *
	 * @param const std::pair<U, U> &
	 * 		The collection range
	 */
	const std::pair<U, U> & getCollectionRange() const {
		return collectionRange;
	}

	/**
	 * Get the active range
	 *
	 * @param const std::pair<U, U> &
	 * 		The active range
	 */
	const std::pair<U, U> & getActiveRange() const {
		return activeRange;
	}

	/**
	 * Get value at current cycle
	 *
	 * 		Sum all the objects in the collection on the current active range start
	 *
	 * @return boost::shared_ptr< T >
	 * 		The sum value on current key
	 */
	boost::shared_ptr<T> getValue() const {
		return (getValue(getActiveRange().first));
	}

	/**
	 * Get value at key val
	 *
	 * 		Sum all the objects in the collection on specified key
	 *
	 * @param U & key
	 * 		The key to calculate the value on
	 *
	 * @return boost::shared_ptr< T
	 * 		The sum object on specified key
	 */
	boost::shared_ptr<T> getValue(const U & key) const {
		return (getValue(key, key));
	}

	/**
	 * Get value at between range key val
	 *
	 * 		Sum all the objects in the collection on specified key range
	 *
	 * @param U & keyStart
	 * 		The start key of range
	 * @param U & keyEnd
	 * 		The end  key of range
	 *
	 * @return boost::shared_ptr< T
	 * 		The sum object on specified key range
	 */
	boost::shared_ptr<T> getValue(const U & keyStart, const U & keyEnd) const {
		boost::shared_ptr<T> sum(new T);
		std::multimap<U, boost::shared_ptr<T> > range = this->getRange(keyStart, keyEnd);
		std::cout<<"StepMappedCollection::getValue: "<<*sum<<std::endl;
		// forall in range
		{
			typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_range = range.begin();
			const typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_range_end = range.end();
			while (it_range != it_range_end) {
				//std::cout<<"StepMappedCollection::getValue: "<<it_range->second->getActivity()<<std::endl;
				*sum += *(it_range->second);
				++it_range;
			}
		}
		return sum;
	}

	/**
	 * Clear collection up to present key
	 *
	 * 		Drop all objects that are 'in the past' relative to that current key;
	 *
	 *	@return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The  collection of deleted impulses
	 */
	std::multimap<U, boost::shared_ptr<T> > clearRange() {
		return (clearRange(this->getActiveRange().second));
	}

	/**
	 * Clear collection up to specified key
	 *
	 * 		Update the collection to specified key (non-inclusive) by dropping all impulses that
	 * are 'in the past' relative to that key. Interval is [0,key)
	 *
	 * @param const T & key
	 * 		The cycle that is the cutoff point for the collection
	 *
	 * @return std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >
	 *		The  collection of deleted impulses
	 */
	std::multimap<U, boost::shared_ptr<T> > clearRange(const U & key) {
		return (this->clearRange(this->getCollectionRange().first, key));
	}

	/**
	 *	Clear the Impulses that start on or after key start parameter and finish before
	 *	key end parameter.  Interval is [key_start,key_end)
	 *
	 *	@param const U & startKey
	 *		Key parameter that marks the start of the cleared area
	 *	 @param const U & endKey
	 *		Key parameter that marks the end of the cleared area (non-inclusive)
	 *
	 *	@return std::multimap<U, boost::shared_ptr< T > >
	 *		The deleted collection of objects
	 */
	std::multimap<U, boost::shared_ptr<T> > clearRange(const U & startKey, const U & endKey) {
		std::cout<<"clearRange: "<<"["<<startKey <<","<<endKey<<")"<<std::endl;
		// switch cycles if start>end
		U range_start = startKey;
		U range_end = endKey;
		if (startKey > endKey) {
			U temp = range_end;
			range_end = range_start;
			range_start = temp;
		}

		std::multimap<U, boost::shared_ptr<T> > cleared_objects = this->getRange(range_start, range_end);
		// actually clear objects
		// forall in cleared_objects
		{
			typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_cleared_objects =
					cleared_objects.begin();
			const typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_cleared_objects_end =
					cleared_objects.end();
			while (it_cleared_objects != it_cleared_objects_end) {
				steppedMap.erase(it_cleared_objects->first);
				++it_cleared_objects;
			}
		}
		return cleared_objects;
	}

	/**
	 * Clear all objects from collection
	 */
	void clear() {
		steppedMap.clear();
	}

	/**
	 * Add object to collection
	 *
	 *@param U key
	 *		Key value to be added
	 * @param boost::shared_ptr<T> obj
	 * 		The object to be added to the collection
	 *
	 * @return boost::shared_ptr<T>
	 * 		The object added to the collection
	 *
	 */
	boost::shared_ptr<T> add(U key, boost::shared_ptr<T> obj) {
		steppedMap.insert(std::pair<U,  boost::shared_ptr<  T > > (key, obj)) ;
		return steppedMap.find(key)->second;
	}

	/**
	 * Remove object from collection
	 *
	 * @param boost::shared_ptr<T> obj
	 * 		The object to be removed from the collection
	 */
	void remove(boost::shared_ptr<T> obj) {
		U keyfound;
		bool found = false;
		// forall in steppedMap
		{
			typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_steppedMap = steppedMap.begin();
			const typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_steppedMap_end = steppedMap.end();
			while (it_steppedMap != it_steppedMap_end && found == false) {
				if (*(it_steppedMap->second) == (*obj)) {
					keyfound = it_steppedMap->first;
					found = true;
				}
				++it_steppedMap;
			}
		}
		this->remove(keyfound);
	}

	/**
	 * Remove object from collection by key
	 *
	 * @param U key
	 * 		The key of the object to be removed from the collection
	 */
	void remove(U key) {
		this->clearRange(key, key+1);
	}

	// GETTERS/SETTERS

	/**
	 * Get underlying collections size
	 *
	 * @return int
	 * 		The collections size
	 */
	int getSize() const {
		return steppedMap.size();
	}

	/**
	 * Get a collection subset that is within a range of keys [start, end)
	 *
	 * @param const U & startKey
	 * 		The start key of range
	 * @param const U & endKey
	 * 		The end key of range
	 *
	 * @return std::multimap<U, boost::shared_ptr<T> >
	 * 		A collection of objects within range
	 */
	std::multimap<U, boost::shared_ptr<T> > getRange(const U & startKey, const U & endKey) const {
		U range_start = startKey;
		U range_end = endKey;
		// end key is non inclusive
		--range_end;

		std::multimap<U, boost::shared_ptr<T> > ranged_objects;
		const std::multimap<U, boost::shared_ptr<T> > & objects = this->getSteppedMap();

		// forall in objects
		{
			typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_objects = objects.lower_bound(range_start);
			const typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_objects_end =
					objects.upper_bound(range_end);
			while (it_objects != it_objects_end) {
				ranged_objects.insert(std::pair<U, boost::shared_ptr<T> >(it_objects->first, it_objects->second));
				++it_objects;
			}
		}
		return ranged_objects;
	}

	/**
	 * Get a mutable collection subset that is within a range of keys [start, end)
	 *
	 * @param const U & startKey
	 * 		The start key of range
	 * @param const U & endKey
	 * 		The end key of range
	 *
	 * @return std::multimap<U, boost::shared_ptr<T> >
	 * 		A collection of objects within range
	 */
	std::multimap<U, boost::shared_ptr<T> > getMutableRange(const U & startKey, const U & endKey)  {
		U range_start = startKey;
		U range_end = endKey;
		// end key is non inclusive
		--range_end;

		std::multimap<U, boost::shared_ptr<T> > ranged_objects;
		std::multimap<U, boost::shared_ptr<T> > & objects = this->getMutableSteppedMap();

		// forall in objects
		{
			typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_objects = objects.lower_bound(range_start);
			const typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_objects_end =
					objects.upper_bound(range_end);
			while (it_objects != it_objects_end) {
				ranged_objects.insert(std::pair<U, boost::shared_ptr<T> >(it_objects->first, it_objects->second));
				++it_objects;
			}
		}
		return ranged_objects;
	}
	friend std::ostream & operator<<(const std::ostream & os, const StepMappedCollection<U,boost::shared_ptr< T > > & obj){
		os<< "StepMappedCollection:"<<&obj<<" { ";
		const std::multimap<U, boost::shared_ptr<T> > & allobjs = obj.getSteppedMap();

		// forall in allobjs
			 {
				 typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_allobjs = allobjs.begin();
				 const  typename std::multimap<U, boost::shared_ptr<T> >::const_iterator it_allobjs_end = allobjs.end();
				 while ( it_allobjs != it_allobjs_end){
					os<< "("<<*(it_allobjs->first)<<","<<*(it_allobjs->second)<<") ";
					 ++it_allobjs;
				 }
			 }
			os<< "}" ;
			 return os;
	}

private:
	/**
	 * Multimap of stepped value/object pairs
	 *
	 * @var std::multimap<U, boost::shared_ptr<T> >
	 */
	std::multimap<U, boost::shared_ptr<T> > steppedMap;

	/**
	 * The collection range
	 *
	 * @var std::pair<U, U>
	 */
	std::pair<U, U> collectionRange;

	/**
	 * The current active range
	 *
	 * @var std::pair<U, U>
	 */
	std::pair<U, U> activeRange;
};

}

}

#endif /* STEPMAPPEDCOLLECTION_H_ */
