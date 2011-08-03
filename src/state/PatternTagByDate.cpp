/*
 * PatternTagByDate.cpp
 *
 *  Created on: 17 Aug 2010
 *      Author: niall
 */

#include "PatternTagByDate.h"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <sstream>

namespace cryomesh {
namespace state {

std::string PatternTagByDate::DateFormat = "%S %M %H %d %m %Y %w %j ";
std::string PatternTagByDate::GlobalStartTag = "";
std::string PatternTagByDate::GlobalEndTag = "";
std::string PatternTagByDate::GlobalCurrentTag = "";
boost::shared_ptr<PatternTagByDate> PatternTagByDate::globalTag; //()=	boost::shared_ptr<PatternTagByDate>(new PatternTagByDate(ByDay));

PatternTagByDate::PatternTagByDate(DateType dt) :
		dateType(dt) {
}

PatternTagByDate::PatternTagByDate(DateType dt, std::tm start_time, std::tm end_time, std::tm current_time) :
		dateType(dt), startTime(start_time), endTime(end_time), currentTime(current_time) {
	globalTag->setStartTag(GlobalStartTag);
	globalTag->setEndTag(GlobalEndTag);
	globalTag->setTag(GlobalStartTag);
	startTime.tm_hour = 3;
}

PatternTagByDate::~PatternTagByDate() {
}

std::string PatternTagByDate::getTag() const {
	return PatternTagByDate::tmToTag(currentTime);
}
void PatternTagByDate::setTag(std::string tg) {
	currentTime = PatternTagByDate::tagToTm(tg);
}
std::string PatternTagByDate::moveTag() {
	if (dateType == ByHour) {
		PatternTagByDate::moveHour(currentTime, 1);
	} else if (dateType == ByDay) {
		PatternTagByDate::moveDay(currentTime, 1);
	} else if (dateType == ByWeek) {
		PatternTagByDate::moveWeek(currentTime, 1);
	} else if (dateType == ByMonth) {
		PatternTagByDate::moveMonth(currentTime, 1);
	} else if (dateType == ByYear) {
		PatternTagByDate::moveYear(currentTime, 1);
	}
	return PatternTagByDate::tmToTag(currentTime);
}
std::string PatternTagByDate::moveTag(int i) {
	for (int j = 0; j < i; j++) {
		this->moveTag();
	}
	return PatternTagByDate::tmToTag(currentTime);
}

std::string PatternTagByDate::getStartTag() const {
	return PatternTagByDate::tmToTag(startTime);
}
void PatternTagByDate::setStartTag(std::string tg) {
	startTime = PatternTagByDate::tagToTm(tg);
}
std::string PatternTagByDate::getEndTag() const {
	return PatternTagByDate::tmToTag(endTime);
}
void PatternTagByDate::setEndTag(std::string tg) {
	endTime = PatternTagByDate::tagToTm(tg);
}

boost::shared_ptr<PatternTag> PatternTagByDate::getGlobalTag() {
	return globalTag;
}

std::tm PatternTagByDate::tagToTm(const std::string & tg) {
	std::tm temp_tm;

	//parse string
	// 'Thu Aug 23 14:55:02 2001'
	boost::tokenizer<> tok(tg);
	boost::tokenizer<>::iterator it_tok;
	boost::tokenizer<>::const_iterator it_tok_end;

	int count = 0;
	//count entries
	{
		it_tok = tok.begin();
		it_tok_end = tok.end();
		//%S %M %H %d %m %Y %w %j dst
		//9 entries
		while (it_tok != it_tok_end) {
			++it_tok;
			++count;
		}
	}
	// assign tm if possible
	if (count == 9) {
		it_tok = tok.begin();
		temp_tm.tm_sec = atoi(it_tok->c_str());
		++it_tok;
		temp_tm.tm_min = atoi(it_tok->c_str());
		++it_tok;
		temp_tm.tm_hour = atoi(it_tok->c_str());
		++it_tok;
		temp_tm.tm_mday = atoi(it_tok->c_str());
		++it_tok;
		temp_tm.tm_mon = atoi(it_tok->c_str()) - 1; //months *since* january so month -1
		++it_tok;
		temp_tm.tm_year = atoi(it_tok->c_str()) - 1900; // years *since* 1900
		++it_tok;
		temp_tm.tm_wday = atoi(it_tok->c_str());
		++it_tok;
		temp_tm.tm_yday = atoi(it_tok->c_str()) - 1; //days *since* january 1st so day number -1
		++it_tok;
		temp_tm.tm_isdst = atoi(it_tok->c_str());
		//temp_tm.tm_zone;
		//temp_tm.tm_gmtoff;
	} else {
		std::cout << "PatternTagByDate::tagToTm: " << "ERROR: Tag doesnt have enough entries! " << std::endl;
	}

	return temp_tm;

}

std::string PatternTagByDate::tmToTag(const std::tm & t) {
	char buffer[80];
	std::strftime(buffer, 80, PatternTagByDate::DateFormat.c_str(), &t);
	std::stringstream ss;
	ss << std::string(buffer) << " " << t.tm_isdst;
	return ss.str();
}
std::tm PatternTagByDate::moveHour(std::tm & tmtime, int i) {
	//convert to time_t
	std::time_t tt = std::mktime(&tmtime);

	//add i* 60 secs * 60 mins =i* 3600 secs

	tt += i * 3600;
	//convert to tm
	std::tm * tmret = std::localtime(&tt);
	return *tmret;

}
std::tm PatternTagByDate::moveDay(std::tm & tmtime, int i) {
	// move 24 hours
	int hours = i * 24;
	return PatternTagByDate::moveHour(tmtime, hours);
}
std::tm PatternTagByDate::moveWeek(std::tm & tmtime, int i) {
	// move 7 days
	int days = i * 7;
	return PatternTagByDate::moveDay(tmtime, days);
}
std::tm PatternTagByDate::moveMonth(std::tm & tmtime, int i) {
	int years = i / 12;
	std::tm month_tm = tmtime;
	if (years > 0) {
		month_tm = PatternTagByDate::moveYear(tmtime, years);
	}
	int days = 0;
	int month = tmtime.tm_mon + 1;
	int months_to_go = i % 12;
	while (months_to_go > 0) {
		int month_days = 0;
		if (month == 2) {
			//check leap year
			bool past_feb = tmtime.tm_mon > 1;
			month_days = 28;

			if (past_feb == true && PatternTagByDate::isLeapYear(tmtime) == true) {
				month_days += 1;
			}
		}
		if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
			month_days = 31;
		} else {
			month_days = 30;
		}
		days += month_days;
		++month;
		--months_to_go;
	}

	return PatternTagByDate::moveDay(month_tm, days);
}
std::tm PatternTagByDate::moveYear(std::tm & tmtime, int years) {
	std::tm presenttime (tmtime);
	const int YEAR_DAYS = 365;

	for (int i = 0; i < years; i++) {
		if (PatternTagByDate::isLeapYear(tmtime) == true) {
			presenttime = PatternTagByDate::moveDay(presenttime, YEAR_DAYS+1);
		} else {
			presenttime =PatternTagByDate::moveDay(presenttime, YEAR_DAYS);
		}
	}
	return presenttime;
}
bool PatternTagByDate::isLeapYear(const std::tm & tmtime) {
	bool leap_year = false;
	int year = 1900 + tmtime.tm_year;

	bool div_by_4 = (year % 4 == 0);
	bool div_by_100 = (year % 100 == 0);
	bool div_by_400 = (year % 400 == 0);
	if (div_by_4 == false) {
		leap_year = false;
	} else if (div_by_100 == false) {
		leap_year = true;
	} else if (div_by_400 == true) {
		leap_year = true;
	} else {
		leap_year = false;
	}
	return leap_year;
}
} //NAMESPACE

} //NAMESPACE
