/*
 * Sequence.cpp
 *
 *  Created on: 24-Jul-2009
 *      Author: niall
 */

#include "Sequence.h"
#include <algorithm>
#include <sstream>
#include <boost/scoped_ptr.hpp>
namespace cryomesh {
namespace state {
const std::string Sequence::INPUT_TAG = "<input>";
const std::string Sequence::OUTPUT_TAG = "<output>";

Sequence::Sequence() {
	initialise();
}
Sequence::~Sequence() {
}
Sequence::Sequence(const Sequence & seq) {

	this->patterns = std::list<std::pair<Pattern, Pattern> >(seq.getPatterns());
	initialise();
}
Sequence::Sequence(const std::vector<Pattern> & input_pats,
		const std::vector<Pattern> & output_pats) {
	if (input_pats.size() == output_pats.size()) {
		std::vector<Pattern>::const_iterator it_input_pats = input_pats.begin();
		const std::vector<Pattern>::const_iterator it_input_pats_end = input_pats.end();
		std::vector<Pattern>::const_iterator it_output_pats = output_pats.begin();
		const std::vector<Pattern>::const_iterator it_output_pats_end = output_pats.end();

		while (it_input_pats != it_input_pats_end && it_output_pats != it_output_pats_end) {
			patterns.push_back(std::pair<Pattern, Pattern>(*it_input_pats, *it_output_pats));
			++it_input_pats;
			++it_output_pats;
		}
		initialise();
	} else {
		std::cout << "Sequence::Sequence: pattern sizes mismatch" << std::endl;
		initialise();
	}
}
Sequence::Sequence(std::ifstream & ifs) {

	loadFromFile(ifs);
	initialise();
}

Sequence & Sequence::operator=(const Sequence & seq) {
	if (&seq == this) {
		return *this;
	}
	initialise();
	this->patterns = std::list<std::pair<Pattern, Pattern> >(seq.getPatterns());
	initialise();
	return *this;
}
const Pattern & Sequence::getCurrentInputPattern() const {
	return this->getCurrentIterator()->first;
}
const Pattern & Sequence::getAndAdvanceCurrentInputPattern()  {
	const Pattern & retpat = this->getCurrentIterator()->first;
	++it_patterns;
	return retpat;
}
const Pattern & Sequence::getCurrentOutputPattern() const {
	return this->getCurrentIterator()->second;
}
const Pattern & Sequence::getAndAdvanceCurrentOutputPattern()  {
	const Pattern & retpat = this->getCurrentIterator()->second;
	++it_patterns;
	return retpat;
}
int Sequence::getCurrentInputPatternId() const {
	return this->getCurrentIterator()->first.getId();

}
int Sequence::getCurrentOutputPatternId() const {
	return this->getCurrentIterator()->second.getId();

}
std::list<std::pair<Pattern, Pattern> >::const_iterator Sequence::getCurrentIterator() const {
	return it_patterns;
}
const Pattern & Sequence::getNextInputPattern() {
	++it_patterns;
	if (this->getCurrentIterator() == patterns.end()) {
		this->setCurrentIterator(this->getPatterns().begin());
	}
	return this->getCurrentInputPattern();
}
const std::list<std::pair<Pattern, Pattern> > & Sequence::getPatterns() const {
	return patterns;
}
const void Sequence::setPatterns(const std::list<std::pair<Pattern, Pattern> > & pats) {
	patterns = std::list<std::pair<Pattern, Pattern> >(pats);
	initialise();
}
void Sequence::saveToFile(std::ofstream & ofs) const {
	std::stringstream ss;

	std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats = this->getPatterns().begin();
	const std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_end =
			this->getPatterns().end();
	while (it_pats != it_pats_end) {
		ss << it_pats->first.getString() << std::endl;
		ss << it_pats->second.getString() << std::endl;
		ss << std::endl;
		++it_pats;
	}
	ofs << ss.str();
	ofs.close();

}
void Sequence::loadFromFile(std::ifstream & ifs) {
	patterns.clear();
	int line_count = 0;
	std::string line_in;
	std::string line_out;
	std::string line_blank;
	// error in stream test
	while (!ifs.eof()) {
		getline(ifs, line_in);
		++line_count;
		getline(ifs, line_out);
		++line_count;
		getline(ifs, line_blank);
		++line_count;
		if (line_in.size() > 0 && line_out.size() > 0 && line_blank.size() == 0) {
			if (line_in.size() == line_out.size()) {
				patterns.push_back(std::pair<Pattern, Pattern>(Pattern(line_in), Pattern(line_out)));
			} else {
				std::cout
						<< "Sequence::loadFromFile: Input/output size mismatch: line: in/out/blank: "
						<< line_count << ":" << line_in << "/" << line_out << "/" << line_blank
						<< std::endl;
			}
		} else {
			std::cout << "Sequence::loadFromFile: File format error: input/output unpaired: line"
					<< line_count << std::endl;
			return;
		}
	}
	initialise();
}
/*
 * Add pattern to sequence
 */
void Sequence::addEntry(Pattern new_in_pat, Pattern new_out_pat) {
	patterns.push_back(std::pair<Pattern, Pattern>(new_in_pat, new_out_pat));
}
/*
 * clear all entries
 */
void Sequence::clear() {
	patterns = std::list<std::pair<Pattern, Pattern> >();
	it_patterns = patterns.begin();
}
/*
 * Match sequences outputs, return double (0,1)
 */
double Sequence::compareOutput(const Sequence & seq) const {
	// deal with trivial cases
	// empty
	if (this->getPatterns().size() == 0) {
		std::cout << "Sequence::compare: this is empty " << std::endl;
		return 0;
	}
	if (seq.getPatterns().size() == 0) {
		std::cout << "Sequence::compare: obj is empty " << std::endl;
		return 0;
	}
	//pattern width mismatch
	if (this->getPatterns().begin()->second.getPattern().size()
			!= seq.getPatterns().begin()->second.getPattern().size()) {
		std::cout << "Sequence::compare: Output pattern width mismatch: this/obj: "
				<< this->getPatterns().begin()->second.getPattern().size() << "/"
				<< seq.getPatterns().begin()->second.getPattern().size() << std::endl;
		return 0;
	}
	// do a simple start to finish comparison of outputs
	double total_match = 0;
	int pat_count = 0;
	const std::list<std::pair<Pattern, Pattern> > & this_patterns = this->getPatterns();
	const std::list<std::pair<Pattern, Pattern> > & obj_patterns = seq.getPatterns();
	//for all in this_patterns
	{
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_this_patterns =
				this_patterns.begin();
		const std::list<std::pair<Pattern, Pattern> >::const_iterator it_this_patterns_end =
				this_patterns.end();
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_obj_patterns =
				obj_patterns.begin();
		const std::list<std::pair<Pattern, Pattern> >::const_iterator it_obj_patterns_end =
				obj_patterns.end();
		while (it_this_patterns != it_this_patterns_end && it_obj_patterns != it_obj_patterns_end) {
			total_match += (*it_this_patterns).second.compare((*it_obj_patterns).second);
			++pat_count;
			++it_obj_patterns;
			++it_this_patterns;
		}
	}
	return (total_match / (double) pat_count);
}
/*
 * Match sequences inputs, return double (0,1)
 */
double Sequence::compareInput(const Sequence & seq) const {
	// deal with trivial cases
	// empty
	if (this->getPatterns().size() == 0) {
		std::cout << "Sequence::compare: this is empty " << std::endl;
		return 0;
	}
	if (seq.getPatterns().size() == 0) {
		std::cout << "Sequence::compare: obj is empty " << std::endl;
		return 0;
	}
	//pattern width mismatch
	if (this->getPatterns().begin()->first.getPattern().size()
			!= seq.getPatterns().begin()->first.getPattern().size()) {
		std::cout << "Sequence::compare: Input pattern width mismatch: this/obj: "
				<< this->getPatterns().begin()->first.getPattern().size() << "/"
				<< seq.getPatterns().begin()->first.getPattern().size() << std::endl;
		return 0;
	}
	// do a simple start to finish comparison of inputs
	double total_match = 0;
	int pat_count = 0;
	const std::list<std::pair<Pattern, Pattern> > & this_patterns = this->getPatterns();
	const std::list<std::pair<Pattern, Pattern> > & obj_patterns = seq.getPatterns();
	//for all in this_patterns
	{
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_this_patterns =
				this_patterns.begin();
		const std::list<std::pair<Pattern, Pattern> >::const_iterator it_this_patterns_end =
				this_patterns.end();
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_obj_patterns =
				obj_patterns.begin();
		const std::list<std::pair<Pattern, Pattern> >::const_iterator it_obj_patterns_end =
				obj_patterns.end();
		while (it_this_patterns != it_this_patterns_end && it_obj_patterns != it_obj_patterns_end) {
			total_match += (*it_this_patterns).first.compare((*it_obj_patterns).first);
			++pat_count;
			++it_obj_patterns;
			++it_this_patterns;
		}
	}
	return (total_match / (double) pat_count);
}

double Sequence::compare(const Sequence & seq) const {
	// deal with trivial cases
	// empty
	if (this->getPatterns().size() == 0) {
		std::cout << "Sequence::compare: this is empty " << std::endl;
		return 0;
	}
	if (seq.getPatterns().size() == 0) {
		std::cout << "Sequence::compare: obj is empty " << std::endl;
		return 0;
	}
	//pattern width mismatch
	if (this->getPatterns().begin()->first.getPattern().size()
			!= seq.getPatterns().begin()->first.getPattern().size()) {
		std::cout << "Sequence::compare: Input pattern width mismatch: this/obj: "
				<< this->getPatterns().begin()->first.getPattern().size() << "/"
				<< seq.getPatterns().begin()->first.getPattern().size() << std::endl;
		return 0;
	}
	if (this->getPatterns().begin()->second.getPattern().size()
			!= seq.getPatterns().begin()->second.getPattern().size()) {
		std::cout << "Sequence::compare: Output pattern width mismatch: this/obj: "
				<< this->getPatterns().begin()->second.getPattern().size() << "/"
				<< seq.getPatterns().begin()->second.getPattern().size() << std::endl;
		return 0;
	}
	// find most simlar inputs and then compare outputs
	// find the biggest sequences map and use that as our fractional match
	int max_size;
	if (this->getPatterns().size() > seq.getPatterns().size()) {
		max_size = this->getPatterns().size();
	} else {
		max_size = seq.getPatterns().size();
	}
	double fraction = (double) 1 / (double) max_size;

	// get  input pattern of sequence that matches the on each of our sequences,
	// note we are allowing multiple matches on each input pattern

	double total_match = 0;

	std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_this =
			this->getPatterns().begin();
	const std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_this_end =
			this->getPatterns().end();

	while (it_pats_this != it_pats_this_end) {
		// if in doubt we match to the beginning
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_best_match =
				seq.getPatterns().begin();
		std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_seq =
				seq.getPatterns().begin();
		const std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_seq_end =
				seq.getPatterns().end();
		double best_input_match = 0;
		while (it_pats_seq != it_pats_seq_end) {
			//find best input match

			double temp_input_match = it_pats_seq->first.compare(it_pats_this->first);
			if (best_input_match < temp_input_match) {
#ifdef SEQUENCE_DEBUG // DEBUG
				std::cout<<"Sequence::compare: best/temp: "<<best_input_match<<"/"<<temp_input_match <<std::endl;
#endif // DEBUG
				best_input_match = temp_input_match;
				it_pats_best_match = it_pats_seq;
			}
			++it_pats_seq;
		}
#ifdef SEQUENCE_DEBUG
		std::cout<<"Sequence::compare: " <<std::endl;
		std::cout<<"in: "<<(it_pats_this->first)<<" out: "<<(it_pats_this->second)<<std::endl;
		std::cout<<"in: "<<(it_pats_best_match->first)<<" out: "<<(it_pats_best_match->second)<<std::endl;
#endif //SEQUENCE_DEBUG
		total_match += ((it_pats_this->second.compare(it_pats_best_match->second)) * fraction);
		++it_pats_this;
	}
	return total_match;
}
double Sequence::compare(int temp_id, const Pattern & pat) const {
	// get  pattern relating to id and compare
	std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats = this->getPatterns().begin();
	const std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_end =
			this->getPatterns().end();
	while (it_pats != it_pats_end) {
		if (it_pats->first.getId() == temp_id) {
			return it_pats->first.compare(pat);
		} else {
			if (it_pats->second.getId() == temp_id) {
				return it_pats->second.compare(pat);
			}
		}
		++it_pats;
	}
	std::cout << "Sequence::compare: No Id match found" << std::endl;
	return 0;
}
bool Sequence::operator==(const Sequence & obj) const {
	bool match = true;
	if (&obj == 0) {
		std::cout << "Sequence::operator ==: obj is null " << std::endl;
		return false;
	}

	// check for no patterns
	if (this->getPatterns().size() == 0) {
		if (obj.getPatterns().size() == 0) {
			return true;
		} else {
			std::cout << "Sequence::operator ==: size mismatch: this is 0;" << std::endl;
			return false;
		}
	} else {
		if (this->getPatterns().size() != obj.getPatterns().size()) {
			std::cout << "Sequence::operator ==: size mismatch " << std::endl;
			return false;
		}
	}
	match = match && std::equal(this->getPatterns().begin(), this->getPatterns().end(),
			obj.getPatterns().begin());
	if (match == false) {
		std::cout << "Sequence::operator ==: patterns mismatch " << std::endl;
		return false;
	}
	return true;

}

std::ostream & operator<<(std::ostream& os, const Sequence & obj) {
	os << "Sequence: " << &obj << std::endl;
	std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats = obj.getPatterns().begin();
	const std::list<std::pair<Pattern, Pattern> >::const_iterator it_pats_end =
			obj.getPatterns().end();
	while (it_pats != it_pats_end) {
		os << it_pats->first.getString() << " : " << it_pats->second.getString() << std::endl;
		++it_pats;
	}
	return os;
}

bool Sequence::isAllZeroes()const{
	//for all in patterns
	{
			std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns = patterns.begin();
		const 	std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns_end = patterns.end();
		while(it_patterns!=it_patterns_end) {
			if ((*it_patterns).first.isAllZeroes() == false || (*it_patterns).second.isAllZeroes() == false){
				return false;
			}
			++it_patterns;
		}
	}
	return true;
}

bool Sequence::isInputAllZeroes() const{
	//for all in patterns
	{
			std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns = patterns.begin();
		const 	std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns_end = patterns.end();
		while(it_patterns!=it_patterns_end) {
			if ((*it_patterns).first.isAllZeroes() == false){
				return false;
			}
			++it_patterns;
		}
	}
	return true;
}

bool Sequence::isOutputAllZeroes()const{
	//for all in patterns
	{
			std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns = patterns.begin();
		const 	std::list<std::pair<Pattern, Pattern> >::const_iterator it_patterns_end = patterns.end();
		while(it_patterns!=it_patterns_end) {
			if ((*it_patterns).second.isAllZeroes() == false){
				return false;
			}
			++it_patterns;
		}
	}
	return true;
}

void Sequence::initialise() {
	// if sequence is empty, always leave us with a default pattern
	//if (this->getPatterns().size() == 0) {
	//	Pattern pat1;
	//	Pattern pat2;
	//	patterns[pat1] = pat2;
	//}
	it_patterns = patterns.begin();
}

void Sequence::setCurrentIterator(std::list<std::pair<Pattern, Pattern> >::const_iterator it) {
	it_patterns = it;
}
}

}
