/*
 * PatternTagById.cpp
 *
 *  Created on: 31 Aug 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "PatternTagById.h"
#include "Pattern.h"
#include <sstream>
namespace cryomesh {
namespace state {

boost::shared_ptr<PatternTagById> PatternTagById::globalTag=	boost::shared_ptr<PatternTagById>(new PatternTagById(1));
PatternTagById::PatternTagById(int d)  {
	std::stringstream ss;
	ss<<d;
	id = ss.str();
}

PatternTagById::~PatternTagById() {
}
std::string PatternTagById::getTag() const{
	return id;
}
void PatternTagById::setTag(std::string tg){
	id =tg;
}
std::string PatternTagById::moveTag(){
	return moveTag(1);
}
std::string PatternTagById::moveTag(int i){
	int newint = atoi(id.c_str());
		newint +=i;
		std::stringstream ss;
			ss<<newint;
			id = ss.str();
		return getTag();
}
std::string PatternTagById::getStartTag() const{
	return "0";
}
std::string PatternTagById::getEndTag() const{
	std::stringstream ss;
			ss<<Pattern::getIds();
			return ss.str();

}

void PatternTagById::setStartTag(std::string tg){
	std::cout<<"PatternTagById::setStartTag: "<<"WARNING: Not setting start tag of PatternTagById to "<<tg<< ", defaults to "<<this->getStartTag()<<std::endl;
}
void PatternTagById::setEndTag(std::string tg){
	std::cout<<"PatternTagById::setEndTag: "<<"WARNING: Not setting end tag of PatternTagById to "<<tg<< ", defaults to "<<this->getEndTag()<<std::endl;
}
boost::shared_ptr<PatternTag> PatternTagById::getGlobalTag(){
	return PatternTagById::globalTag;
}
}

}
