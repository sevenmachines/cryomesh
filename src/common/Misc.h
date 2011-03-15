/*
 * Misc.h
 *
 *  Created on: 5 Nov 2010
 *      Author: niall
 */

#ifndef MISC_H_
#define MISC_H_

#include <boost/tokenizer.hpp>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>
#include <boost/uuid/uuid.hpp>
#include <cctype>
#include <map>
#include <boost/shared_ptr.hpp>

namespace cryomesh {

namespace common {

/**
 * Misc contains a collection of useful miscellaneous functions
 */
class Misc {
public:
	Misc();
	virtual ~Misc();

	template<class T>
	static std::string getMapUUIDPointerString(const std::map<boost::uuids::uuid, boost::shared_ptr< T> >& obj) {
		// forall in obj
		std::stringstream ss;
		{
			typename std::map<boost::uuids::uuid, boost::shared_ptr< T> >::const_iterator it_obj = obj.begin();
			const typename std::map<boost::uuids::uuid, boost::shared_ptr< T> >::const_iterator it_obj_end = obj.end();
			while (it_obj != it_obj_end) {
				ss << "uuid:" <<formatUUID(it_obj->first)<<std::endl;
				ss<<"obj: "<< *(it_obj->second);
				//std::cout<<"Misc::printContainer: "<<*it_obj<<std::endl;
				++it_obj;
			}
		}
		return ss.str();
	}

	/*
	 * Take a string and tokenize to a container
	 */
	static std::list<std::string> parseArgs(const std::string & str) {

		std::list<std::string> store;
		boost::tokenizer<> tok(str);

		// forall in tok
		{
			boost::tokenizer<>::const_iterator it_tok = tok.begin();
			const boost::tokenizer<>::const_iterator it_tok_end = tok.end();
			while (it_tok != it_tok_end) {
				store.push_back(*it_tok);
				++it_tok;
			}
		}
		return store;
	}

	template<class T>
	static std::string getContainerString(const std::list<T> & obj) {
		// forall in obj
		std::stringstream ss;
		{
			typename std::list<T>::const_iterator it_obj = obj.begin();
			const typename std::list<T>::const_iterator it_obj_end = obj.end();
			while (it_obj != it_obj_end) {
				ss << " " << *it_obj;
				//std::cout<<"Misc::printContainer: "<<*it_obj<<std::endl;
				++it_obj;
			}
		}
		return ss.str();
	}

	static std::ostream & print (std::ostream & os, const boost::uuids::uuid & id) {
		std::stringstream ss;
		// forall in id
		{
			boost::uuids::uuid::const_iterator it_id = id.begin();
			const boost::uuids::uuid::const_iterator it_id_end = id.end();
			while (it_id != it_id_end) {
				ss << *it_id;
				++it_id;
			}
		}
		os<<ss.str();
		return os;
	}

	static std::ostream & print(std::ostream& os, const std::vector<bool> & obj) {
		//os << "std::vector<bool> " << obj.size() << ": ";

		// forall in obj
		{
			std::vector<bool>::const_iterator it_obj = obj.begin();

			const std::vector<bool>::const_iterator it_obj_end = obj.end();
			while (it_obj != it_obj_end) {
				if (*it_obj == true) {
					os << "1";
				} else if (*it_obj == false) {
					os << "0";
				}
				++it_obj;
			}
		}
		return os;
	}

	static std::ostream & print(std::ostream& os, const std::list<bool> & obj) {
			//os << "std::vector<bool> " << obj.size() << ": ";

			// forall in obj
			{
				std::list<bool>::const_iterator it_obj = obj.begin();

				const std::list<bool>::const_iterator it_obj_end = obj.end();
				while (it_obj != it_obj_end) {
					if (*it_obj == true) {
						os << "1";
					} else if (*it_obj == false) {
						os << "0";
					}
					++it_obj;
				}
			}
			return os;
		}

	static std::string toLower(const std::string & str) ;
	static std::list<std::string> toLower(const std::list<std::string> & strlist) ;
	static std::string removeChars(const std::string & delchars, const std::string & str);
};


}

}

#endif /* MISC_H_ */
