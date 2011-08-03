/*
 * Sequence.h
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 *
 *      Sequence objects hold list of Pattern's and allow for easy comparison and
 *      manipulation within hydra systems
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Pattern.h"
#include <vector>
#include <fstream>
#include <ostream>
#include <string>
#include <list>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>

namespace cryomesh {
namespace state {
class Sequence {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & patterns;
	}
public:
	/*
	 * Constructor for an empty sequence
	 */
	Sequence();
	/*
	 * Destructor for Sequence
	 */
	virtual ~Sequence();
	/*
	 * Copy constructor for Sequence
	 */
	Sequence(const Sequence & seq);
	/*
	 * Constructor for Sequence from vector of Patterns
	 */
	Sequence(const std::vector<Pattern> & input_pats, const std::vector<Pattern> & output_pats);
	/*
	 * Constructor for Sequence from file
	 */
	Sequence(std::ifstream & ifs);

	Sequence & operator=(const Sequence & seq);

	/*
	 * get the current pattern
	 */
	const Pattern & getCurrentInputPattern() const;
	const Pattern & getCurrentOutputPattern() const;
	const Pattern & getAndAdvanceCurrentInputPattern();
	const Pattern & getAndAdvanceCurrentOutputPattern();
	int getCurrentInputPatternId() const;
	int getCurrentOutputPatternId() const;
	/*
	 * get iterator to current position
	 */
	std::list<std::pair<Pattern, Pattern> >::const_iterator getCurrentIterator() const;
	void setCurrentIterator(std::list<std::pair<Pattern, Pattern> >::const_iterator);
	/*
	 * Move to the next pattern and return it
	 */
	const Pattern & getNextInputPattern();
	/*
	 * Getter for Patterns container
	 */
	const std::list<std::pair<Pattern, Pattern> > & getPatterns() const;
	/*
	 * Setter for Patterns container
	 */
	 void setPatterns(const std::list<std::pair<Pattern, Pattern> > & pats);
	/*
	 * Save the sequence data to a file in plain text
	 */
	void saveToFile(std::ofstream & ofs) const;
	/*
	 * Retrieve a sequence from a plain text file
	 */
	void loadFromFile(std::ifstream & ifs);
	/*
	 * Add pattern to sequence
	 */
	void addEntry(Pattern new_in_pat, Pattern new_out_pat);
	/*
	 * clear all entries
	 */
	void clear();
	/*
	 * Match sequences outputs, return double (0,1)
	 */
	double compareOutput(const Sequence & seq) const;
	/*
	 * Match sequences inputs, return double (0,1)
	 */
	double compareInput(const Sequence & seq) const;
	/*
	 * Match sequences, return double (0,1)
	 */
	double compare(const Sequence & seq) const;
	/*
	 * Match the Pattern associated with the id to another
	 * Pattern, return double (0,1)
	 */
	double compare(int id, const Pattern & pat) const;
	/*
	 * Sequence equals comparison
	 */
	bool operator==(const Sequence & obj) const;
	bool isAllZeroes()const;
	bool isInputAllZeroes()const;
	bool isOutputAllZeroes()const;
	static const std::string INPUT_TAG;
	static const std::string OUTPUT_TAG;
	friend std::ostream & operator<<(std::ostream& os, const Sequence & obj);

private:
	/*
	 * Map of a Pattern id/pattern (in) to Pattern id/pattern (out)
	 */
	std::list<std::pair<Pattern, Pattern> > patterns;
	/*
	 * Iterator for the map
	 */
	std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns;

	/*
	 * Common contructor jobs
	 */
	void initialise();

};

}

}
#endif /* SEQUENCE_H_ */
