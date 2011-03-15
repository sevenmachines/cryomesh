#include "Misc.h"
#include <cstdio>

namespace cryomesh {

namespace common {
std::string Misc::toLower(const std::string & str) {
	// forall in str
	std::stringstream ss;
	{
		std::string::const_iterator it_str = str.begin();
		const std::string::const_iterator it_str_end = str.end();
		while (it_str != it_str_end) {
			char c = std::tolower(*it_str);
			ss << c;
			++it_str;
		}
	}
	return ss.str();
}
std::list<std::string> Misc::toLower(const std::list<std::string> & strlist) {
	std::list<std::string> templist;
	// forall in strlist
	{
		std::list<std::string>::const_iterator it_strlist = strlist.begin();
		const std::list<std::string>::const_iterator it_strlist_end = strlist.end();
		while (it_strlist != it_strlist_end) {
			templist.push_back(Misc::toLower(*it_strlist));
			++it_strlist;
		}
	}
	return templist;

}
std::string Misc::removeChars(const std::string & delchars, const std::string & str) {
	std::string str_copy = str;
	// forall in delchars

	{
		std::string::const_iterator it_delchars = delchars.begin();
		const std::string::const_iterator it_delchars_end = delchars.end();
		while (it_delchars != it_delchars_end) {
			size_t found = str_copy.find(*it_delchars);
			while (found != std::string::npos) {
				str_copy.erase(found, 1);
				found = str_copy.find(*it_delchars);
			}
			++it_delchars;
		}
	}
	return str_copy;
}

}//NAMESPACE
}//NAMESPACE
