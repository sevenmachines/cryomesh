/*
 * PatternTagByDate.h
 *
 *  Created on: 17 Aug 2010
 *      Author: niall
 */

#ifndef PATTERNTAGBYDATE_H_
#define PATTERNTAGBYDATE_H_

#include "PatternTag.h"
#include <ctime>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
namespace cryomesh {
namespace state {

class PatternTagByDate: public PatternTag {
public:
	enum DateType {
		ByHour, ByDay, ByWeek, ByMonth, ByYear
	};

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar &  globalTag;
		ar &  dateType;
		ar &  startTime;
		ar &  endTime;
		ar &  currentTime;
	}

	PatternTagByDate(DateType dt);
	PatternTagByDate(DateType dt, std::tm start_time, std::tm end_time,
			std::tm current_time);
	virtual ~PatternTagByDate();

	virtual std::string getTag() const; // get identifying tag
	virtual void setTag(std::string tg);
	virtual std::string moveTag(); // Move to next tag and return implementation ie, if date this would be tomorrow
	virtual std::string moveTag(int i); // Move to next tag x i and return implementation ie, if date this would be tomorrow
	virtual std::string getStartTag() const; // return the default starting tag
	virtual void setStartTag(std::string tg);
	virtual std::string getEndTag() const; // return the default starting tag
	virtual void setEndTag(std::string tg);

	virtual boost::shared_ptr<PatternTag> getGlobalTag();

	static std::tm tagToTm(const std::string & tg);
	static std::string tmToTag(const std::tm & t);

	static std::tm moveHour(std::tm & ttime, int i);
	static std::tm moveDay(std::tm & ttime, int i);
	static std::tm moveWeek(std::tm & ttime, int i);
	static std::tm moveMonth(std::tm & ttime, int i);
	static std::tm moveYear(std::tm & ttime, int i);
	static bool isLeapYear(const std::tm & tmtime);

protected:
	static std::string GlobalStartTag;
	static std::string GlobalEndTag;
	static std::string GlobalCurrentTag;
	static std::string DateFormat;

private:
	static boost::shared_ptr<PatternTagByDate> globalTag;
	DateType dateType;
	std::tm startTime;
	std::tm endTime;
	std::tm currentTime;

};

}

}


#endif /* PATTERNTAGBYDATE_H_ */
