/*
 * PatternChannel.h
 *
 *  Created on: 18 Aug 2010
 *      Author: niall
 */

#ifndef PATTERNCHANNEL_H_
#define PATTERNCHANNEL_H_

#include "Pattern.h"
#include "PatternTag.h"
#include "common/Tagged.h"
#include "common/Debuggable.h"
#include <map>
#include <list>
#include <set>

#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>

namespace cryomesh {
namespace state {

class PatternChannel: public common::Tagged, public common::Debuggable {
public:
	enum PrintFormat {
		BINARY, TEXT, INTEGER
	};
	enum ChannelDataType {
		Input, Output, Transitional
	};
	/*
	 * Contruct with no pattern
	 */
	PatternChannel(ChannelDataType dt);

	/*
	 * Construct from pattern
	 */
	PatternChannel(const std::list<boost::shared_ptr<Pattern> > & pats, ChannelDataType dt);
	virtual ~PatternChannel();

	PatternChannel(const PatternChannel & obj);
	/**
	 * Assignment operator
	 *
	 * @param const PatternChannel & obj
	 * 		RHS assignment
	 *
	 * @return PatternChannel &
	 * 		This object after assignment
	 */
	PatternChannel & operator=(const PatternChannel & obj);

	// add and remove patterns
	/*
	 * add single pattern to the end
	 */
	void addPattern(boost::shared_ptr<Pattern> pat);
	/*
	 * Add list of patterns to end
	 */
	void addPatterns(const std::list<boost::shared_ptr<Pattern> > & pats);
	/*
	 * Add list of patterns starting at position
	 */
	void addPatterns(const std::list<boost::shared_ptr<Pattern> > & pats, int position);
	/*
	 * Remove list of patterns
	 */
	std::list<boost::shared_ptr<Pattern> > removePatterns(const std::list<boost::shared_ptr<Pattern> > & pats);
	/*
	 * remove list of patterns by uuid
	 */
	std::list<boost::shared_ptr<Pattern> > removePatterns(const std::list<boost::uuids::uuid> & uuids);
	/*
	 * Clear pattern list and all related settings are reset
	 */
	void clearPatternList();
	/*
	 * Force Channel size contraint by dropping the earliest patterns
	 */
	std::list<boost::shared_ptr<Pattern> > forcePatternListSize(int sz);
	/*
	 * Force Channel size contraint by dropping the earliest patterns using default max size
	 */
	std::list<boost::shared_ptr<Pattern> > forcePatternListSize();
	// Manipulation
	/*
	 * return current pattern
	 */
	const boost::shared_ptr<Pattern> getCurrentPattern();

	/*
	 * Return bactracked pattern using last entry in pattern list as that
	 * of the current cycle
	 *
	 * @param Cycle
	 * 	The cycle to retrieve relative to now
	 *
	 * @return boost::shared_ptr<Pattern>
	 * 	The pattern found at the supplied cycle, null if none found
	 */
	const boost::shared_ptr<Pattern> getPatternByCycle(const common::Cycle & cycle);

	/*
	 * Return pattern from tag
	 */
	const boost::shared_ptr<Pattern> getPatternByTag(const boost::shared_ptr<PatternTag> tg) const;
	/*
	 * Move to next pattern and return it
	 */
	const boost::shared_ptr<Pattern> nextPattern();
	/*
	 * Move to previous pattern and return it
	 */
	const boost::shared_ptr<Pattern> previousPattern();

	/*
	 * Try to match this pattern channel with another. from 0 to 1(full match)
	 * Global match, constantly go through the pattern list removing the best match
	 * before proceeding
	 */
	double matchGlobally(const PatternChannel &);

	/*
	 * Try to match this pattern channel with another. from 0 to 1(full match)
	 * Sequential match, match each pattern with its equivalent in the other
	 * pattern channel then proceed sequntally through both channels
	 */
	double matchSequentially(const PatternChannel &);
	virtual void enableDebug(bool b);

	// GETTERS/SETTERS
	ChannelDataType getChannelDataType() const;
	int getLength() const;
	const std::map<boost::shared_ptr<PatternTag>, boost::uuids::uuid>
	& getPatternByTagMap() const;
	const boost::shared_ptr<Pattern> getPatternByUUID(const boost::uuids::uuid & id) const;
	boost::shared_ptr<Pattern> getMutablePatternByUUID(const boost::uuids::uuid & id);
	const std::list<boost::uuids::uuid> & getPatternList() const;
	std::list<boost::uuids::uuid>::iterator getPatternListIterator() const;
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >
	& getPatternMap() const;
	int getPatternPosition() const;
	int getWidth() const;
	void setWidth(int w);
	int getRefID() const;
	void setRefID(int r);

	int getMaxPatternListSize() const;
	void setMaxPatternListSize(int sz);
	std::ostream & printPatternList(std::ostream & os, bool reversed = false) const;

	std::ostream & printTextFormattedPatternList(std::ostream & os, bool reversed = false) const;
	std::ostream & printIntegerFormattedPatternList(std::ostream & os, bool reversed) const;
	std::ostream & printBinaryFormattedPatternList(std::ostream & os, bool reversed) const;
	std::ostream & printFormattedPatternList(std::ostream & os, const PrintFormat & pf, bool reversed) const;

	static const int DEFAULT_MAX_PATTERN_LIST_SIZE;

	static const unsigned int REFID_CREATE_START;
	static unsigned int refIDS;
	static unsigned int getRefIDS();

	friend std::ostream & operator<<(std::ostream & os, const PatternChannel & obj);
private:
	ChannelDataType channelDataType;
	// auto generated from pattern list
	int length;
	int maxPatternListSize;
	// Map of a useful pattern tag to an actual pattern, for example, date
	std::map<boost::shared_ptr<PatternTag>, boost::uuids::uuid> patternByTagMap;
	// Ordered sequential list of Pattern uuids
	std::list<boost::uuids::uuid> patternList;
	// Current position in patternList
	std::list<boost::uuids::uuid>::iterator patternListIterator;
	// Contains map of Pattern uuids to actual Patterns
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > patternMap;
	// Position count of patternList
	int patternPosition;
	// reference id taken from xml file
	int refID;
	// auto generated from pattern list using minimum element width
	int width;

};

}

}

#endif /* PATTERNCHANNEL_H_ */
