/*
 * Pattern.h
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 *
 *      Pattern objects are wrapper objects for containers of bools in some sense
 *      It allows for comparison and manipulation to ease the generation and use of
 *      different patterns
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include "PatternTag.h"
#include "BinaryString.h"
#include "common/Tagged.h"
#include <vector>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "manager/PatternDatabaseObject.h"

namespace cryomesh {
namespace state {
class Pattern :public common::Tagged{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & id;
		ar & ids;
		ar & binaryString;
		//ar & tag;
	}
public:
	/*
	 * Constructor to create an empty object
	 */
	Pattern();
	/*
	 * Copy constructor
	 */
	Pattern(const Pattern &);
	/*
	 * Constructor to create an object from a bit string
	 */
	Pattern(const std::string &);
	/*
	 * Constructor to create an object from a vector of bools
	 */
	Pattern(const std::vector<bool> &);
	/*
	 * Constructor to create an object from a BinaryString
	 */
	Pattern(const BinaryString & obj);
	virtual ~Pattern();

	Pattern& operator=(const Pattern &);

	/*
	 * Compare one pattern to another using their vector<bool> return a match double(0,1)
	 */
	double compare(const Pattern &) const;
	bool isAllZeroes() const;
	// worker methods
	bool operator==(const Pattern &) const;
	bool operator<(const Pattern &) const;
	// get/set
	std::vector<bool> getPattern() const;
	std::string getString() const;
	void setPattern(const std::vector<bool> &);
	boost::uuids::uuid getUUID() const;
	int getId() const;
	void setId(int);
	int getWidth() const;
	int getSize() const;
const BinaryString & getBinaryString() const;
	BinaryString & getMutableBinaryString();

	const boost::shared_ptr<PatternTag> getPatternTag() const;
	void setPatternTag(boost::shared_ptr<PatternTag> pt);

	boost::shared_ptr<manager::DatabaseObject> getDatabaseObject() const ;

	static int getIds();
	static void setIds(int);

	// some useful static methods
	/*
	 * Method to take a vector<bool> and turn it into a bit style string
	 */
	static std::string patternToString(const std::vector<bool> & vec);
	/*
	 * Method to take a  bit style string  and turn it into a vector<bool>
	 */
	static std::vector<bool> stringToPattern(const std::string &);

	/**
	 * Generate a random pattern of width
	 *
	 * @param unsigned int
	 * 		Width of pattern to generate
	 *@param double
	 *		Fraction of pattern to activate, default to 0.5
	 * 	@return Pattern
	 * 		The random pattern
	 */
	static Pattern getRandom(unsigned int width, double fraction=0.5);

	friend std::ostream & operator<<(std::ostream& os, const Pattern & obj);

private:
	// vector to hold the actual pattern we represent
	BinaryString binaryString;
	// unique-ish id
	int id;

	// identifying tag for this
	boost::shared_ptr<PatternTag> patternTag;

	boost::uuids::uuid tag;
	/*
	 * Common constructor jobs
	 */
	void initialise();

	// statics
	static int ids;
};

}

}

#endif /* PATTERN_H_ */
