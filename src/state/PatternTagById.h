/*
 * PatternTagById.h
 *
 *  Created on: 31 Aug 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef PATTERNTAGBYID_H_
#define PATTERNTAGBYID_H_
#include "PatternTag.h"
namespace cryomesh {
namespace state {

class PatternTagById: public PatternTag {
public:
	PatternTagById(int d);
	virtual ~PatternTagById();
	virtual std::string getTag() const; // get identifying tag
	virtual void setTag(std::string tg);
	virtual std::string moveTag(); // Move to next tag and return implementation ie, if date this would be tomorrow
	virtual std::string moveTag(int i); // Move to next tag x i and return implementation ie, if date this would be tomorrow
	virtual std::string getStartTag() const; // return the default starting tag
	virtual void setStartTag(std::string tg);
	virtual std::string getEndTag() const; // return the default starting tag
	virtual void setEndTag(std::string tg);

	virtual boost::shared_ptr<PatternTag> getGlobalTag();

private:
	static boost::shared_ptr<PatternTagById> globalTag;
	std::string id;
};

}

}

#endif /* PATTERNTAGBYID_H_ */
