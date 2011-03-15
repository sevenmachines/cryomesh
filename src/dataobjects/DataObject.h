/*
 * DataObject.h
 *
 *  Created on: 3 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef DATAOBJECT_H_
#define DATAOBJECT_H_

#include <map>
#include <ostream>

namespace cryomesh {

namespace dataobjects {

/**
 * Class to contain all the useful data about an object. Useful for output and plotting
 */
template<class U, class T>
class DataObject {
public:
	enum ComparisonType {
		MaximumValue, MinimumValue, EqualityValue, AverageValue
	};
	/**
	 * Default contructor
	 */
	DataObject() :
		loggingEnabled(false), datasetMaximumSize(DEFAULT_DATASET_SIZE) {
	}

	/**
	 * Contructor with max size
	 */
	DataObject(unsigned int sz) :
		loggingEnabled(false), datasetMaximumSize(sz) {
	}

	/**
	 * Default destructor
	 */
	virtual ~DataObject() {
	}

	/**
	 * Whether logging is enabled or not
	 *
	 * @param bool enable
	 * 		True to enable logging, false otherwise
	 */
	void enableLogging(bool enable) {
		loggingEnabled = enable;
	}
	/**
	 * Check logging is enabled or not
	 *
	 * @return bool enable
	 * 		Trueif logging enabled, flase otherwise
	 */
	bool isLoggingEnabled() {
		return loggingEnabled;
	}

	/**
	 * Get all cycle values
	 *
	 * @return std::map<unsigned long int, double> &
	 * 		The cycle values
	 */
	const std::map<U, T> &getMap() const {
		return valueMap;
	}

	/**
	 * Get all mutable cycle values
	 *
	 * @return std::map<U, T> &
	 * 		The mutable cycle values
	 */
	std::map<U, T>& getMutableMap() {
		return valueMap;
	}

	/**
	 * Get all  values within a range [start, end]
	 *
	 * @param U start
	 * 		The start cycle of the range
	 * 	@param U end
	 * 		The end cycle of the range
	 *
	 * @return std::map<unsigned long int, double>
	 * 		The cycle values
	 */
	const std::map<U, T> getMap(U start, U end) const {
		typename std::map<U, T>::const_iterator it_start = valueMap.lower_bound(start);
		typename std::map<U, T>::const_iterator it_end = valueMap.upper_bound(end);
		std::map<U, T> temp_map(it_start, --it_end);
		return temp_map;
	}

	/**
	 * Get all  values within a range [start, ]
	 *
	 * @param U start
	 * 		The start cycle of the range
	 *
	 * @return std::map<unsigned long int, double>
	 * 		The cycle values
	 */
	const std::map<U, T> getMap(U start) const {
		typename std::map<U, T>::const_iterator it_start = valueMap.lower_bound(start);
		typename std::map<U, T>::const_iterator it_end = valueMap.end();
		std::map<U, T> temp_map(it_start, it_end);
		return temp_map;
	}

	/**
	 * Get value from key
	 *
	 * @param U key
	 * 		The key to find
	 *
	 * @return T
	 * 		The value found
	 */
	const T getByKey(U key) const {
		T keyfound;
		typename std::map<U, T>::const_iterator it_found = valueMap.find(key);
		typename std::map<U, T>::const_iterator it_end = valueMap.end();
		if (it_found != it_end) {
			keyfound = it_found->second;
		}
		return keyfound;
	}

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const DataObject & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const DataObject & obj) {
		os << "DataObject:" << "enabled:" << obj.loggingEnabled;
		os << "{ ";
		// forall in valueMap
		{
			typename std::map<U, T>::const_iterator it_valueMap = obj.valueMap.begin();
			const typename std::map<U, T>::const_iterator it_valueMap_end = obj.valueMap.end();
			while (it_valueMap != it_valueMap_end) {
				os << "(" << it_valueMap->first << "," << it_valueMap->second << ") ";
				++it_valueMap;
			}
		}
		os << " }";
		return os;
	}

	/**
	 * Clear all data
	 */
	void clear() {
		valueMap.clear();
	}

	/**
	 * Add entry
	 *
	 * @param unsigned int cycle
	 * 		The cycle the value is on
	 * 	@param double
	 * 		The value
	 *
	 */
	void insert(U key, T object) {
		valueMap[key] = object;
		//	std::cout<<"DataObject::insert: "<<"("<<key<<","<<object<<")"<<std::endl;
		int diff = valueMap.size() - getDatasetMaximumSize();

		if (diff > 0) {
			typename std::map<U, T>::iterator it_valueMap = valueMap.begin();
			typename std::map<U, T>::iterator it_valueMap_begin = valueMap.begin();

			const typename std::map<U, T>::const_iterator it_valueMap_end = valueMap.end();

			while (diff > 0) {
				--diff;
				++it_valueMap;
			}
			//	std::cout<<"DataObject::insert: "<<"oldsize:"<<valueMap.size();
			valueMap.erase(it_valueMap_begin, it_valueMap);
			//	std::cout<<" newsize:"<<valueMap.size() <<std::endl;
			//assert(valueMap.size() <=  getDatasetMaximumSize());
		}
	}

	unsigned int getDatasetMaximumSize() const {
		return datasetMaximumSize;
	}

	void setDatasetMaximumSize(unsigned int sz) {
		datasetMaximumSize = sz;
	}

	/**
	 * Get comparison values
	 *
	 * @param ComparisonType type
	 * 		The type of comparison to make
	 *
	 * 	@return T
	 * 		The result of the comparison
	 */
	T getValueComparison(ComparisonType type) const {
		T val;
		int count = 0;

		// forall in valueMap
		{
			typename std::map<U, T>::const_iterator it_valueMap = valueMap.begin();
			const typename std::map<U, T>::const_iterator it_valueMap_end = valueMap.end();
			while (it_valueMap != it_valueMap_end) {
				if (count == 0) {
					val = it_valueMap->second;
				} else if (type == MaximumValue && val < it_valueMap->second) {
					val = it_valueMap->second;
				} else if (type == MinimumValue && val > it_valueMap->second) {
					val = it_valueMap->second;
				} else if (type == AverageValue) {
					val += it_valueMap->second;
				}
				++count;
				++it_valueMap;
			}
		}
		if (type == AverageValue){
			val =val/valueMap.size();
		}
		return val;
	}

	/**
	 * Get maximum value
	 *
	 * @return T
	 * 		The resultant value
	 */
	T getMaximumValue()const{
		return (this->getValueComparison(MaximumValue));
	}

	/**
	 * Get minimum value
	 *
	 * @return T
	 * 		The resultant value
	 */
	T getMinimumValue()const{
		return (this->getValueComparison(MinimumValue));
	}

	/**
	 * Get average value value
	 *
	 * @return T
	 * 		The resultant value
	 */
	T getAverageValue()const{
		return (this->getValueComparison(AverageValue));
	}
protected:
	/**
	 * Map of cycle to value
	 *
	 * @var std::map<unsigned long int, double>
	 */
	std::map<U, T> valueMap;

	/**
	 * True of logging is enabled, false otherwise
	 *
	 * @var bool
	 */
	bool loggingEnabled;

	/**
	 * Maximum size of logged dataset
	 *
	 * @var unsigned int
	 */
	unsigned int datasetMaximumSize;

	/**
	 * Default size of logged dataset
	 *
	 * @var unsigned int
	 */
	static const unsigned int DEFAULT_DATASET_SIZE;
};
template<class U, class T>
const unsigned int dataobjects::DataObject<U, T>::DEFAULT_DATASET_SIZE = 100;

}

}

#endif /* DATAOBJECT_H_ */
