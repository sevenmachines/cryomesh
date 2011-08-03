/*
 * PatternTag.h
 *
 *  Created on: 17 Aug 2010
 *      Author: niall
 */

#ifndef PATTERNTAG_H_
#define PATTERNTAG_H_

#include <string>
#include <boost/shared_ptr.hpp>

namespace cryomesh {
namespace state {

class PatternTag {
public:
	virtual std::string getTag() const=0; // get identifying tag
	virtual void setTag(std::string tg)=0;
	virtual std::string moveTag()=0; // Move to next tag and return implementation ie, if date this would be tomorrow
	virtual std::string moveTag(int i)=0; // Move to next tag x i and return implementation ie, if date this would be tomorrow
	virtual std::string getStartTag() const=0; // return the default starting tag
	virtual std::string getEndTag() const=0; // return the default starting tag
	virtual void setStartTag(std::string tg)=0;
	virtual void setEndTag(std::string tg)=0;

	virtual boost::shared_ptr<PatternTag> getGlobalTag()=0;

};

}

}


#endif /* PATTERNTAG_H_ */
