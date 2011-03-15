/*
 * BinaryString.cpp
 *
 *  Created on: 26 Nov 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

//#define BINARYSTRING_DEBUG
#include "BinaryString.h"
#include "common/Misc.h"
#include <cstdio>
#include <iostream>
#include <assert.h>

namespace cryomesh {
namespace state {

const unsigned int BinaryString::BINARY_CHAR_LENGTH = 8;
const int BinaryString::MAX_BINARY_INTEGER_SIZE = 32;

BinaryString::BinaryString() :
	binaryString(""), signBit(false) {

}
BinaryString::BinaryString(const std::string & str, bool sign_bit, Type tp) :
	signBit(sign_bit) {

	// if its just a seeries of 1 and 0's
	if (tp == BIN) {
		binaryString = str;
		//std::cout << "BinaryString::BinaryString: " << "Straight binary conversion: " << binaryString << std::endl;

	} else {
		std::list<int> allins = BinaryString::formatTextToInts(str);
		std::stringstream ss;
		// forall in allins
		{
			int count = 0;
			std::stringstream tempss;
			std::list<int>::const_iterator it_allins = allins.begin();
			const std::list<int>::const_iterator it_allins_end = allins.end();
			while (it_allins != it_allins_end) {
				tempss.clear();
				tempss << BinaryString::charToBinaryString(*it_allins, this->getSignBit());
				std::string tempstr = tempss.str();
				BinaryString workbin(tempstr, sign_bit, BIN);
				workbin.resize(BinaryString::BINARY_CHAR_LENGTH);
				ss << workbin;
				assert(workbin.getBinaryString().size() == 8);
				//	std::cout << "BinaryString::BinaryString: " << "count: " << count << ": " << ss.str() << std::endl;
				++count;
				++it_allins;
			}
		}
		binaryString = (ss.str());
	}

}
BinaryString::BinaryString(const std::vector<bool> & binvec, bool sign_bit) :
	signBit(sign_bit) {
	std::stringstream ss;
	// forall in binvec
	{
		std::vector<bool>::const_iterator it_binvec = binvec.begin();
		const std::vector<bool>::const_iterator it_binvec_end = binvec.end();
		while (it_binvec != it_binvec_end) {
			if (*it_binvec ==false) {
				ss << "0";
			} else {
				ss << "1";
			}
			++it_binvec;
		}
	}
	binaryString = ss.str();
}

BinaryString::BinaryString(const BinaryString & obj) {
	binaryString = obj.getBinaryString();
	signBit = obj.getSignBit();
}

BinaryString::~BinaryString() {

}

void BinaryString::setSignBit(bool b) {
	signBit = b;
}
bool BinaryString::getSignBit() const {
	return signBit;
}
const std::string & BinaryString::getBinaryString() const {
	return binaryString;
}

const std::vector<bool> BinaryString::getBools() const {
	std::vector<bool> binvec;
	if (this->isValidBinary() != true) {
		std::cout << "BinaryString::getBinaryStringAsBools: " << "WARNING: Not a valid binary" << std::endl;
		return binvec;
	}
	// forall in binaryString
	{
		std::string::const_iterator it_binaryString = binaryString.begin();
		const std::string::const_iterator it_binaryString_end = binaryString.end();
		while (it_binaryString != it_binaryString_end) {
			if (*it_binaryString == '0') {
				binvec.push_back(false);
			} else {
				binvec.push_back(true);
			}
			++it_binaryString;
		}
	}
#ifdef BINARYSTRING_DEBUG
	std::cout<<"BinaryString::getBools: "<<binaryString<<std::endl;
	common::Misc::print(std::cout, binvec);
	std::cout<<std::endl;
#endif
	return binvec;

}

void BinaryString::setBinaryString(const std::string & str) {
	binaryString = str;
}
void BinaryString::setBinaryString(const std::vector<bool> & binvec) {
	std::cout<<"BinaryString::setBinaryString: "<<""<<std::endl;
	common::Misc::print(std::cout, binvec);
	std::stringstream ss;
	// forall in binvec
	{
		std::vector<bool>::const_iterator it_binvec = binvec.begin();
		const std::vector<bool>::const_iterator it_binvec_end = binvec.end();
		while (it_binvec != it_binvec_end) {
			if (*it_binvec == false) {
				ss << "0";
			} else {
				ss << "1";
			}
			++it_binvec;
		}
	}
	binaryString = ss.str();
}
bool BinaryString::isValidBinary() const {
	//std::cout<<"BinaryString::isValidBinary: "<<binaryString<<std::endl;
	// forall in binaryString
	{
		std::string::const_iterator it_binaryString = binaryString.begin();
		const std::string::const_iterator it_binaryString_end = binaryString.end();
		while (it_binaryString != it_binaryString_end) {
			if (*it_binaryString != '0' && *it_binaryString != '1') {
				return false;
			}
			++it_binaryString;
		}
	}
	return true;
}
bool BinaryString::isAllZeroes() const {
	// forall in binaryString
	{
		std::string::const_iterator it_binaryString = binaryString.begin();
		const std::string::const_iterator it_binaryString_end = binaryString.end();
		while (it_binaryString != it_binaryString_end) {
			if (*it_binaryString == '1') {
				return false;
			}
			++it_binaryString;
		}
	}
	return true;
}

unsigned int BinaryString::getWidth() const {
	return binaryString.size();
}

int BinaryString::toInt() const {

	//std::cout << "BinaryString::toInt: " << binaryString << std::endl;
	// check if we need to chop to fit int size
	std::string binstr_copy = binaryString;
	if (binstr_copy.size()>MAX_BINARY_INTEGER_SIZE){
		binstr_copy.resize(MAX_BINARY_INTEGER_SIZE);
		std::cout<<"BinaryString::toInt: "<<"WARNING: Chopping BinaryString length "<< binaryString.size()<< " to fit maximum length "<<MAX_BINARY_INTEGER_SIZE<<std::endl;
	}
	if (this->isValidBinary() != true) {
		std::cout << "BinaryString::toInt: " << "ERROR: Invalid Binary" << std::endl;
		return 0;
	}
	int powerof2 = 1;
	int number = 0;
	int str_sz = binstr_copy.size();
	if (signBit == true) {
		--str_sz;
	}
	for (int i = str_sz - 1; i >= 0; i--) {
		std::stringstream ss;
		ss << binstr_copy.at(i);
		int val = atoi(ss.str().c_str());
		//std::cout << "formatBinaryStringToInt: " << "i: " << i << " val: " << val << std::endl;
		number += powerof2 * val;
		powerof2 *= 2;
	}

	if (this->getSignBit() == true) {
		// get sign bit
		const char * ch = &binstr_copy.at(binstr_copy.size() - 1);
		int val = atoi(ch);
		if (val == 1) {
			number = -number;
		}
	}
#ifdef BINARYSTRING_DEBUG
	std::cout<<"BinaryString::toInt: number: "<<number<<std::endl;
#endif
	return number;
}

std::ostream & operator<<(std::ostream& os, const BinaryString & obj) {
	os << obj.getBinaryString();
	return os;
}
std::list<int> BinaryString::toInts() const {
	std::string allbins;
	std::list<int> allints;
	std::list<BinaryString> binstrs = BinaryString::formatTextToBinaryStrings(this->getBinaryString(), allbins);
	// forall in binstrs
	{
		std::list<BinaryString>::const_iterator it_binstrs = binstrs.begin();
		const std::list<BinaryString>::const_iterator it_binstrs_end = binstrs.end();
		while (it_binstrs != it_binstrs_end) {
			allints.push_back(it_binstrs->toInt());
			++it_binstrs;
		}
	}
	return allints;
}
std::string BinaryString::toText() const {
	std::stringstream ss;
	//std::cout << "BinaryString::toText: binaryString: " << binaryString << std::endl;
	// forall in str
	{
		std::string::const_iterator it_str = binaryString.begin();
		const std::string::const_iterator it_str_end = binaryString.end();
		while (it_str != it_str_end) {
			std::stringstream binss;
			binss.clear();
			binss.flush();
			for (int i = 0; i < BINARY_CHAR_LENGTH && it_str != it_str_end; i++) {
				//std::cout << "BinaryString::toText: i: " << i << " = " << *it_str << std::endl;
				binss << *it_str;
				++it_str;
			}
			std::string tmpstr = binss.str();
	//		std::cout << "BinaryString::toText: tmpstr: " << tmpstr << std::endl;
			BinaryString binstr(tmpstr, this->getSignBit(), BIN);
			binstr.resize(BINARY_CHAR_LENGTH);
			if (binstr.getBinaryString().size() != BINARY_CHAR_LENGTH) {
				std::cout << "BinaryString::toText: ERROR: binstr is size " << binstr.getBinaryString().size()
						<< " != " << BINARY_CHAR_LENGTH << std::endl;
				assert(binstr.getBinaryString().size() == 8);
			}
			int binint = binstr.toInt();
			char binchar(binint);
			ss << binchar;
		}
	}

	return (ss.str());
}

std::list<int> BinaryString::formatTextToInts(const std::string & str) {
	std::list<int> charints;
	// forall in str
	{
		std::string::const_iterator it_str = str.begin();
		const std::string::const_iterator it_str_end = str.end();
		while (it_str != it_str_end) {
			charints.push_back(*it_str);
			++it_str;
		}
	}
	return charints;
}
std::string BinaryString::formatIntsToText(const std::list<int> & charints) {
	std::stringstream ss;
	// forall in charints
	{
		std::list<int>::const_iterator it_charints = charints.begin();
		const std::list<int>::const_iterator it_charints_end = charints.end();
		while (it_charints != it_charints_end) {
			char tempchar = *it_charints;
			ss << tempchar;
			++it_charints;
		}
	}
	return ss.str();
}
std::list<BinaryString> BinaryString::formatTextToBinaryStrings(const std::string & str, std::string & allbins) {
	std::list<BinaryString> parsedStr;
	std::stringstream allbins_ss;

	// forall in str
	{
		std::string::const_iterator it_str = str.begin();
		const std::string::const_iterator it_str_end = str.end();
		while (it_str != it_str_end) {
			char ch = *it_str;
			std::string binstr = BinaryString::charToBinaryString(ch, false);
			BinaryString bs(binstr, false, BinaryString::BIN);
			//assert(bs.getBinaryString().size()%BinaryString::BINARY_CHAR_LENGTH ==0);
			parsedStr.push_back(bs);
			allbins_ss << bs;
			++it_str;
		}
	}
	allbins = allbins_ss.str();
	return parsedStr;

}

std::string BinaryString::charToBinaryString(const char & ch, bool sign_bit) {
	int num = ch;

	int decimal = num;
	char binary[1024];
	int k = 0, n = 0;
	int neg_flag = 0;
	int remain;
	int old_decimal; // for test
	char temp[80];

	// take care of negative input
	if (sign_bit == true && decimal < 0) {
		decimal = -decimal;
		neg_flag = 1;
	}
	do {
		old_decimal = decimal; // for test
		remain = decimal % 2;
		// whittle down the decimal number
		decimal = decimal / 2;
		// this is a test to show the action
		//printf("%d/2 = %d remainder = %d\n", old_decimal, decimal, remain);
		// converts digit 0 or 1 to character '0' or '1'
		temp[k++] = remain + '0';
	} while (decimal > 0);

	if (sign_bit == true) {
		if (neg_flag) {
			temp[k++] = '1'; // add - sign
		} else {
			temp[k++] = '0'; // space
		}
	}
	// reverse the spelling
	while (k >= 0)
		binary[n++] = temp[--k];

	binary[n - 1] = 0; // end with NULL
	std::string tempstr(binary);
	//std::cout << "BinaryString::BinaryString(int): " << tempstr << std::endl;
	std::string binstring = std::string(binary);
	int mod = binstring.size() % BINARY_CHAR_LENGTH;
	int res_sz = binstring.size() + (BINARY_CHAR_LENGTH - mod);
	BinaryString tempbs(binstring, false, BinaryString::BIN);
	binstring = tempbs.resize(res_sz);
	assert( binstring.size() % BINARY_CHAR_LENGTH ==0);
	return binstring;
}
std::string BinaryString::resize(const unsigned int sz) {
	if (this->getBinaryString().size() == sz) {
		return this->getBinaryString();
	}
	int size = sz;
	if (this->getSignBit() == true) {
		size = sz - 1;
	}

	std::stringstream revss;
	std::string last_bit;
	// forall in str
	{

		std::string::const_reverse_iterator it_str = binaryString.rbegin();
		last_bit = *it_str;
		const std::string::const_reverse_iterator it_str_end = binaryString.rend();
		while (it_str != it_str_end) {
			revss << *it_str;
			++it_str;
		}
	}

	std::string revstr = revss.str();
	revstr.resize(size, '0');
	if (this->getSignBit() == true) {
		revstr.resize(size + 1, last_bit.at(0));
	}
	// now re-reverse it!
	std::stringstream newss;
	// forall in revstr
	{
		std::string::const_reverse_iterator it_revstr = revstr.rbegin();
		const std::string::const_reverse_iterator it_revstr_end = revstr.rend();
		while (it_revstr != it_revstr_end) {
			newss << *it_revstr;
			++it_revstr;
		}
	}

	// readjustment since we've preserved sign bit or not.
	std::string newstr = newss.str();
	if (newstr.size() != sz) {
		std::cout << "BinaryString::resize: ERROR in resize(). New string " << "\"" << newstr << "\"" << " size is "
				<< newstr.size() << ", should be " << sz << std::endl;
		assert(newstr.size() == sz);
	}
	binaryString = newstr;
	return newstr;
}

}//NAMESPACE
}//NAMESPACE
