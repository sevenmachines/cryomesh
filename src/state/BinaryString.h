/*
 * BinaryString.h
 *
 *  Created on: 26 Nov 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef BINARYSTRING_H_
#define BINARYSTRING_H_
#include <list>
#include <string>
#include <sstream>
#include <vector>

#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
namespace cryomesh {
namespace state {

class Pattern;
class BinaryString {
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & binaryString;
		ar & signBit;
	}
	enum Type {
		TXT, BIN
	};
	BinaryString();
	BinaryString(const std::string & str, bool sign_bit = false, Type tp = TXT);
	BinaryString(const std::vector<bool> & binvec, bool sign_bit);
	BinaryString(const BinaryString & obj);
	virtual ~BinaryString();

	void setSignBit(bool b);
	bool getSignBit() const;
	const std::string & getBinaryString() const;
	const std::vector<bool> getBools() const;

	void setBinaryString(const std::string & str);
	void setBinaryString(const std::vector<bool> & binvec) ;

	bool isValidBinary() const;
	bool isAllZeroes() const;
	unsigned int getWidth()const;

	std::list<int> toInts() const;
	std::string toText() const;
	std::string resize(const unsigned int size);
	int toInt() const;
	friend std::ostream & operator<<(std::ostream& os, const BinaryString & obj);

	static std::list<int> formatTextToInts(const std::string & str);
	static std::string formatIntsToText(const std::list<int> & charints);

	static std::list<BinaryString> formatTextToBinaryStrings(const std::string & str, std::string & allbins);
	static std::string formatBinaryStringsToText(const std::list<std::string> & strs);
	static const unsigned int BINARY_CHAR_LENGTH;
	static const int MAX_BINARY_INTEGER_SIZE;
private:
	std::string binaryString;
	bool signBit;

	//private helpers

	static std::string charToBinaryString(const char & ch, bool sign_bit);

};

}

}


#endif /* BINARYSTRING_H_ */
