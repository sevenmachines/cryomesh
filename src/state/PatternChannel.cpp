/*
 * PatternChannel.cpp
 *
 *  Created on: 18 Aug 2010
 *      Author: niall
 */

#include "PatternChannel.h"
#include <boost/foreach.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "common/TimeKeeper.h"

namespace cryomesh {
namespace state {

const unsigned int PatternChannel::REFID_CREATE_START = 100000;
unsigned int PatternChannel::refIDS = PatternChannel::REFID_CREATE_START;
const int PatternChannel::DEFAULT_MAX_PATTERN_LIST_SIZE = -1;

unsigned int PatternChannel::getRefIDS() {
	++PatternChannel::refIDS;
	return PatternChannel::refIDS;
}
PatternChannel::PatternChannel(ChannelDataType dt) :
	width(0), length(0), patternPosition(0), channelDataType(dt) {
	patternListIterator = patternList.begin();
	refID = PatternChannel::getRefIDS();
	maxPatternListSize = DEFAULT_MAX_PATTERN_LIST_SIZE;
	// warn if we have 0 size and Input or vice versa
	if (channelDataType == PatternChannel::Input) {
		std::cout << "PatternChannel::PatternChannel: " << "WARNING: Empty pattern list but set as Input " << std::endl;
	}
#ifdef PATTERNCHANNEL_DEBUG
	std::cout << "PatternChannel::PatternChannel(ChannelDataType): Created " << "uuid: " << this->getUUIDString()
	<< std::endl;
#endif
}
PatternChannel::PatternChannel(const std::list<boost::shared_ptr<Pattern> > & pats, ChannelDataType dt) :
	width(0), length(0), patternPosition(0), channelDataType(dt) {
	maxPatternListSize = DEFAULT_MAX_PATTERN_LIST_SIZE;
	if (pats.size() > 0) {
		this->addPatterns(pats);
	}
	patternListIterator = patternList.begin();
	refID = PatternChannel::getRefIDS();

	// warn if we have 0 size and Input or vice versa
	if (pats.size() <= 0 && channelDataType == PatternChannel::Input) {
		std::cout << "PatternChannel::PatternChannel: " << "WARNING: Empty pattern list but set as Input " << std::endl;
	} else if (pats.size() > 0 && channelDataType == PatternChannel::Output) {
		std::cout << "PatternChannel::PatternChannel: " << "WARNING: Have pattern list but set as Output " << std::endl;
	}
#ifdef PATTERNCHANNEL_DEBUG
	std::cout << "PatternChannel::PatternChannel(std::list<boost::shared_ptr<Pattern> >, ChannelDataType): Created "
	<< "uuid: " << this->getUUIDString() << std::endl;
#endif
}

PatternChannel::~PatternChannel() {
#ifdef PATTERNCHANNEL_DEBUG
	std::cout << "PatternChannel::~PatternChannel: Destroying " << "uuid: " << this->getUUIDString() << std::endl;
#endif
}

PatternChannel::PatternChannel(const PatternChannel & obj)  : Tagged(obj.getUUID()){
	Tagged(obj.getUUID());
	this->refID = obj.refID;
	this->uuid = obj.uuid;
	this->width = obj.width;
	this->length = obj.length;
	this->maxPatternListSize = obj.maxPatternListSize;
	this->patternList = obj.patternList;
	this->patternMap = obj.patternMap;
	this->patternListIterator = obj.patternListIterator;
	this->patternPosition = obj.patternPosition;
	this->patternByTagMap = obj.patternByTagMap;
	this->channelDataType = obj.channelDataType;
}
PatternChannel & PatternChannel::operator=(const PatternChannel & obj) {
	if (this != &obj) {
		this->refID = obj.refID;
		this->uuid = obj.uuid;
		this->width = obj.width;
		this->length = obj.length;
		this->maxPatternListSize = obj.maxPatternListSize;
		this->patternList = obj.patternList;
		this->patternMap = obj.patternMap;
		this->patternListIterator = obj.patternListIterator;
		this->patternPosition = obj.patternPosition;
		this->patternByTagMap = obj.patternByTagMap;
		this->channelDataType = obj.channelDataType;
	}
	return *this;
}

void PatternChannel::addPattern(boost::shared_ptr<Pattern> pat) {
	std::list<boost::shared_ptr<Pattern> > temp_list;
	temp_list.push_back(pat);
	this->addPatterns(temp_list, this->getLength() + 1);
}
void PatternChannel::addPatterns(const std::list<boost::shared_ptr<Pattern> > & pats) {
	this->addPatterns(pats, this->getLength() + 1);
}

void PatternChannel::addPatterns(const std::list<boost::shared_ptr<Pattern> > & pats, int position) {

	// clmp position. if <1 then insert at start, if >sz insert at end
	std::list<boost::uuids::uuid>::iterator it_pos;
	if (position < 1) {
		it_pos = patternList.begin();
	} else if (position > this->getLength()) {
		it_pos = patternList.end();
	} else {
		it_pos = patternList.begin();
		for (int i = 1; i < position; i++) {
			++it_pos;
		}
	}

	unsigned int pats_added = 0;
	// copy objects
	boost::shared_ptr<Pattern> pat(new Pattern());
	BOOST_FOREACH(pat, pats)
				{
					// reduce width to new minimum
					if (pat->getWidth() < this->getWidth() || this->getWidth() == 0) {
						this->setWidth(pat->getWidth());
					}
					patternMap[pat->getUUID()] = pat;
					patternByTagMap[pat->getPatternTag()] = pat->getUUID();
					if (position == -1) {
						patternList.push_back(pat->getUUID());
					} else if (position == 0) {
						patternList.push_front(pat->getUUID());
					} else {
						patternList.insert(it_pos, pat->getUUID());
					}
					// TODO patternByTag[pat->getTag()]
					++pats_added;
					++length;
				}
	// if the list was empty ar first, set start to 1
	if (pats_added == patternList.size()) {
		patternPosition = 1;
		patternListIterator = patternList.begin();
	} else if (position >= 1 && position <= patternPosition) {
		// rearrange markers if they are after the insertion point
		patternPosition += pats_added;
	}

	// force size
	this->forcePatternListSize();
}

std::list<boost::shared_ptr<Pattern> > PatternChannel::removePatterns(
		const std::list<boost::shared_ptr<Pattern> > & pats) {

	// just pass on to UUID deletion
	std::list<boost::uuids::uuid> temp_ids;

	//for all in pats
	{
		std::list<boost::shared_ptr<Pattern> >::const_iterator it_pats = pats.begin();
		const std::list<boost::shared_ptr<Pattern> >::const_iterator it_pats_end = pats.end();
		while (it_pats != it_pats_end) {
			temp_ids.push_back((*it_pats)->getUUID());
			++it_pats;
		}
	}

	return (this->removePatterns(temp_ids));
}

std::list<boost::shared_ptr<Pattern> > PatternChannel::removePatterns(const std::list<boost::uuids::uuid> & uuids) {

	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_pat;
	std::list<boost::shared_ptr<Pattern> > deleted_pats;
	boost::uuids::uuid uuid;
	BOOST_FOREACH(uuid, uuids)
				{
					it_pat = patternMap.find(uuid);
					if (it_pat != patternMap.end()) {
						// if found, save
						deleted_pats.push_back(patternMap[uuid]);

						// TODO patternByTag.erase[ patternMap[UUID]->getByTag()];
						// and delete
						patternMap.erase(uuid);
						patternList.remove(uuid);
						patternByTagMap.erase(it_pat->second->getPatternTag());

					}
				}
	length = patternList.size();
	//reset counters to beginning
	patternListIterator = patternList.begin();
	patternPosition = 1;
	return deleted_pats;
}
void PatternChannel::clearPatternList() {
	patternByTagMap.clear();
	patternList.clear();
	patternMap.clear();
	width = 0;
	length = 0;
	patternPosition = 0;
	patternListIterator = patternList.begin();
}
std::list<boost::shared_ptr<Pattern> > PatternChannel::forcePatternListSize(int sz) {
	if (sz >= 0) {
		//uuids to delete
		std::list<boost::uuids::uuid> del_uuids;
		//check size
		int overspill = patternList.size() - sz;
		if (overspill > 0) {
			std::list<boost::uuids::uuid>::const_iterator it_patternList = patternList.begin();
			const std::list<boost::uuids::uuid>::const_iterator it_patternList_end = patternList.end();

			while (overspill > 0 && it_patternList != it_patternList_end) {
				//get enough uuids for earilest patterns
				del_uuids.push_back(*it_patternList);
				--overspill;
				++it_patternList;
			}

			// remove patterns and return those lost
			return this->removePatterns(del_uuids);
		}
	}
	std::list<boost::shared_ptr<Pattern> > empty_list;
	return empty_list;
}
std::list<boost::shared_ptr<Pattern> > PatternChannel::forcePatternListSize() {
#ifdef PATTERNCHANNEL_DEBUG
std::cout << "PatternChannel::forcePatternListSize: " << this->getMaxPatternListSize() << std::endl;
#endif
	return (this->forcePatternListSize(this->getMaxPatternListSize()));
}
// Manipulation

const boost::shared_ptr<Pattern> PatternChannel::getCurrentPattern() {
	return patternMap.find(*patternListIterator)->second;
}

const boost::shared_ptr<Pattern> PatternChannel::getPatternByCycle(const common::Cycle & cycle){
	boost::shared_ptr<Pattern> found_pattern;

	std::list<boost::uuids::uuid>::const_reverse_iterator it_diff_iterator = patternList.rbegin();
	const std::list<boost::uuids::uuid>::const_reverse_iterator it_diff_iterator_end = patternList.rend();

	 common::Cycle cycle_diff = common::TimeKeeper::getTimeKeeper().getCycle() - cycle;
#ifdef PATTERNCHANNEL_DEBUG
	 std::cout<<"PatternChannel::getPatternByCycle: "<<"diff="<<cycle_diff <<" now="<< common::TimeKeeper::getTimeKeeper().getCycle() <<" get_cycle="<< cycle<<std::endl;
#endif
	while (cycle_diff > 0 && (it_diff_iterator != it_diff_iterator_end)){
		--cycle_diff;
		++it_diff_iterator;
	}

	if (it_diff_iterator != it_diff_iterator_end){
		found_pattern =patternMap.find(*it_diff_iterator)->second;
	}else{
		std::cout<<"PatternChannel::getPatternByCycle: "<<"Warning: Cycle "<<cycle<<" not found in Pattern List size "<<patternList.size()<<std::endl;
	}
	return found_pattern;
}

const boost::shared_ptr<Pattern> PatternChannel::getPatternByTag(const boost::shared_ptr<PatternTag> tg) const {
	boost::shared_ptr<Pattern> found_pat;
	const std::map<boost::shared_ptr<PatternTag>, boost::uuids::uuid>::const_iterator it_find =
			patternByTagMap.find(tg);
	if (it_find != patternByTagMap.end()) {
		found_pat = (patternMap.find(it_find->second))->second;
	}
	return found_pat;
}
const boost::shared_ptr<Pattern> PatternChannel::nextPattern() {
	if (this->getLength() == 1) {
		patternPosition = 1;
		patternListIterator = patternList.begin();
	} else if (patternPosition != 0) {
		++patternListIterator;
		++patternPosition;
	} else {
		patternListIterator = patternList.begin();
		if (patternList.size() > 0) {
			patternPosition = 1;
		}
	}

	if (patternListIterator == patternList.end()) {
		patternListIterator = patternList.begin();
		patternPosition = 1;
	}
	return this->getCurrentPattern();
}
const boost::shared_ptr<Pattern> PatternChannel::previousPattern() {
	if (patternListIterator == patternList.begin()) {
		patternListIterator = patternList.end();
		patternPosition = patternList.size() + 1;
	}
	--patternListIterator;
	--patternPosition;

	return this->getCurrentPattern();
}

double PatternChannel::matchGlobally(const PatternChannel & obj) {
	double total_match = 0;
	unsigned int pat_count = 0;

	const std::list<boost::uuids::uuid> & thislist = this->getPatternList();
	const int this_pat_count = thislist.size();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > & thismap = this->getPatternMap();
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_thisfound = thismap.end();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_thismap_end = thismap.end();

	const std::list<boost::uuids::uuid> & objlist = obj.getPatternList();
	const int obj_pat_count = objlist.size();

	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > & objmap = obj.getPatternMap();
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > objmapused;


	const unsigned int min_list_sz = std::min(this_pat_count, obj_pat_count);
	// forall in thislist
	// do this in reverse to check in reverse chronological order
	{
		unsigned int usedmatches = 0;
		std::list<boost::uuids::uuid>::const_reverse_iterator it_thislist = thislist.rbegin();
		const std::list<boost::uuids::uuid>::const_reverse_iterator it_thislist_end = thislist.rend();

		// end oncw we reach the end of list or have found enough matches
		while (it_thislist != it_thislist_end && objmapused.size() != objmap.size()) {
			// marker count for found matches

			// forall uuids get a pattern reference
			it_thisfound = thismap.find(*it_thislist);
			if (it_thisfound == it_thismap_end) {
				std::cout << "PatternChannel::matchSequentially: "
						<< "ERROR: Cannot find this uuid in this pattern map" << std::endl;
				assert(false);
			}

			// iterate through objmap loooking for the bestmatch
			// forall in objmap
			{
				double best_match = -1;
				std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objmap_bestmatch =
						objmap.end();
				std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objmap = objmap.begin();
				const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objmap_end =
						objmap.end();
				while (it_objmap != it_objmap_end) {
					double temp_match = it_thisfound->second->compare(*(it_objmap->second));
					if (temp_match > best_match) {
						//check we havent already used this
						std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objmapused_found =
								objmapused.find(it_objmap->first);
						if (it_objmapused_found == objmapused.end()) {
							best_match = temp_match;
							it_objmap_bestmatch = it_objmap;
						}
					}
					++it_objmap;
				}

				// now add the booster
				total_match += best_match;
				// and mark the matching object as used
				objmapused[it_objmap_bestmatch->first] = it_objmap_bestmatch->second;
			}
			if (objmapused.size() != usedmatches + 1) {
				std::cout << "PatternChannel::matchGlobally: "
						<< "ERROR: Used obj map and used matches differ: objmapused: " << objmapused.size()
						<< " usedmatches: " << usedmatches << std::endl;
				assert(objmapused.size() == usedmatches+1);
			}

			++usedmatches;
			++it_thislist;
			++pat_count;
		}

		if (min_list_sz != pat_count) {
			std::cout << "PatternChannel::matchSequentially: ERROR: Not found expected number of patterns: exp: "
					<< this_pat_count << " act: " << pat_count << "" << std::endl;
			assert(min_list_sz == pat_count);
		}
	}

	return (total_match / (double) pat_count);
}

double PatternChannel::matchSequentially(const PatternChannel & obj) {
	double total_match = 0;
	unsigned int pat_count = 0;

	const std::list<boost::uuids::uuid> & thislist = this->getPatternList();
	const unsigned int this_pat_count = thislist.size();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > & thismap = this->getPatternMap();
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_thisfound = thismap.end();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_thismap_end = thismap.end();

	const std::list<boost::uuids::uuid> & objlist = obj.getPatternList();
	const unsigned int obj_pat_count = objlist.size();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > & objmap = obj.getPatternMap();
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objfound = objmap.end();
	const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_objmap_end = objmap.end();

	if (this_pat_count != obj_pat_count) {
		std::cout << "PatternChannel::matchSequentially: " << "Error: This and obj pat_counts differ: "
				<< this_pat_count << "/" << obj_pat_count << std::endl;
		assert(false);
	}
	// forall in thislist
	// Match in reverse chronological order
	{
		std::list<boost::uuids::uuid>::const_reverse_iterator it_thislist = thislist.rbegin();
		const std::list<boost::uuids::uuid>::const_reverse_iterator it_thislist_end = thislist.rend();
		std::list<boost::uuids::uuid>::const_reverse_iterator it_objlist = objlist.rbegin();
		const std::list<boost::uuids::uuid>::const_reverse_iterator it_objlist_end = objlist.rend();

		while (it_thislist != it_thislist_end && it_objlist != it_objlist_end) {
			// forall uuids get a pattern reference
			it_thisfound = thismap.find(*it_thislist);
			if (it_thisfound == it_thismap_end) {
				std::cout << "PatternChannel::matchSequentially: "
						<< "ERROR: Cannot find this uuid in this pattern map" << std::endl;
				assert(false);
			}

			it_objfound = objmap.find(*it_objlist);
			if (it_objfound == it_objmap_end) {
				std::cout << "PatternChannel::matchSequentially: " << "ERROR: Cannot find obj uuid in this pattern map"
						<< std::endl;
				assert(false);
			}

			total_match += it_thisfound->second->compare(*(it_objfound->second));

			++it_thislist;
			++it_objlist;
			++pat_count;
		}

		if (this_pat_count != pat_count) {
			std::cout << "PatternChannel::matchSequentially: ERROR: Not found expected number of patterns: exp: "
					<< this_pat_count << " act: " << pat_count << "" << std::endl;
			assert(false);
		}
	}

	return (total_match / (double) pat_count);
}

// GETTERS/SETTERS
int PatternChannel::getLength() const {
	return patternList.size();
}

const std::map<boost::shared_ptr<PatternTag>, boost::uuids::uuid> & PatternChannel::getPatternByTagMap() const {
	return patternByTagMap;
}

const std::list<boost::uuids::uuid> & PatternChannel::getPatternList() const {
	return patternList;
}

std::list<boost::uuids::uuid>::iterator PatternChannel::getPatternListIterator() const {
	return patternListIterator;
}

const std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> > & PatternChannel::getPatternMap() const {
	return patternMap;
}

int PatternChannel::getPatternPosition() const {
	return patternPosition;
}

int PatternChannel::getWidth() const {
	return width;
}
void PatternChannel::setWidth(int w) {
	width = w;

}

int PatternChannel::getRefID() const {
	return refID;
}
void PatternChannel::setRefID(int r) {
	std::cout << "PatternChannel::setRefID: " << "WARNING: Multiple same refid's allowed.." << std::endl;
	refID = r;
}

PatternChannel::ChannelDataType PatternChannel::getChannelDataType() const {
	return channelDataType;
}

 int PatternChannel::getMaxPatternListSize() const {
	return maxPatternListSize;
}
void PatternChannel::setMaxPatternListSize(int sz) {
	if (maxPatternListSize > sz) {
		this->forcePatternListSize(sz);
	}
	maxPatternListSize = sz;
}

const boost::shared_ptr<Pattern> PatternChannel::getPatternByUUID(const boost::uuids::uuid & id) const {
	boost::shared_ptr<Pattern> temppat;
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_patternMap_found = patternMap.find(id);
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_patternMap_found_end =
			patternMap.end();

	if (it_patternMap_found != it_patternMap_found_end) {
		temppat = it_patternMap_found->second;
	}
	return temppat;
}

boost::shared_ptr<Pattern> PatternChannel::getMutablePatternByUUID(const boost::uuids::uuid & id) {
	boost::shared_ptr<Pattern> temppat;
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_patternMap_found = patternMap.find(id);
	std::map<boost::uuids::uuid, boost::shared_ptr<Pattern> >::const_iterator it_patternMap_found_end =
			patternMap.end();

	if (it_patternMap_found != it_patternMap_found_end) {
		temppat = it_patternMap_found->second;
	}
	return temppat;
}

void PatternChannel::enableDebug(bool b){
	 this->setDebug(b);
 }

std::ostream & PatternChannel::printPatternList(std::ostream & os, bool reversed) const {
	if (reversed == false) {
		// forall in patternList
		{

			std::list<boost::uuids::uuid>::const_iterator it_patternList = patternList.begin();
			const std::list<boost::uuids::uuid>::const_iterator it_patternList_end = patternList.end();
			while (it_patternList != it_patternList_end) {
				const boost::shared_ptr<Pattern> temppat = this->getPatternByUUID(*it_patternList);
				if (temppat != 0) {
					os << temppat->getString() << std::endl;
				}
				++it_patternList;
			}
		}
	} else {
		std::list<boost::uuids::uuid>::const_reverse_iterator it_patternList = patternList.rbegin();
		const std::list<boost::uuids::uuid>::const_reverse_iterator it_patternList_end = patternList.rend();
		while (it_patternList != it_patternList_end) {
			const boost::shared_ptr<Pattern> temppat = this->getPatternByUUID(*it_patternList);
			if (temppat != 0) {
				os << temppat->getString() << std::endl;
			}
			++it_patternList;
		}

	}
	return os;
}
std::ostream & PatternChannel::printTextFormattedPatternList(std::ostream & os, bool reversed) const {
	return this->printFormattedPatternList(os, TEXT, reversed);

}
std::ostream & PatternChannel::printIntegerFormattedPatternList(std::ostream & os, bool reversed) const {
	return this->printFormattedPatternList(os, INTEGER, reversed);
}
std::ostream & PatternChannel::printBinaryFormattedPatternList(std::ostream & os, bool reversed) const {
	return this->printFormattedPatternList(os, BINARY, reversed);
}
std::ostream & PatternChannel::printFormattedPatternList(std::ostream & os, const PrintFormat & pf, bool reversed) const {
	if (reversed == false) {
		// forall in patternList
		{

			std::list<boost::uuids::uuid>::const_iterator it_patternList = patternList.begin();
			const std::list<boost::uuids::uuid>::const_iterator it_patternList_end = patternList.end();
			while (it_patternList != it_patternList_end) {
				boost::shared_ptr<Pattern> temppat = this->getPatternByUUID(*it_patternList);
				if (temppat != 0) {
					if (pf == BINARY) {
						os << temppat->getBinaryString() << std::endl;
						//std::cout << "PatternChannel::printFormattedPatternList: " << "BINARY" << std::endl;

					} else if (pf == TEXT) {
						//std::cout << "PatternChannel::printFormattedPatternList: " << "TEXT" << std::endl;

						os << temppat->getBinaryString().toText() << std::endl;

					} else if (pf == INTEGER) {
						//std::cout << "PatternChannel::printFormattedPatternList: " << "INTEGER" << std::endl;
						os << temppat->getBinaryString().toInt() << std::endl;
					} else {
						//default to raw binary
						os << temppat->getBinaryString() << std::endl;
					}
				}
				++it_patternList;
			}
		}
	} else {
		std::list<boost::uuids::uuid>::const_reverse_iterator it_patternList = patternList.rbegin();
		const std::list<boost::uuids::uuid>::const_reverse_iterator it_patternList_end = patternList.rend();
		while (it_patternList != it_patternList_end) {
			boost::shared_ptr<Pattern> temppat = this->getPatternByUUID(*it_patternList);
			if (temppat != 0) {
				if (pf == BINARY) {
					os << temppat->getBinaryString() << std::endl;
					//std::cout << "PatternChannel::printFormattedPatternList: " << "BINARY" << std::endl;

				} else if (pf == TEXT) {
					//std::cout << "PatternChannel::printFormattedPatternList: " << "TEXT" << std::endl;

					os << temppat->getBinaryString().toText() << std::endl;

				} else if (pf == INTEGER) {
					//	std::cout << "PatternChannel::printFormattedPatternList: " << "INTEGER" << std::endl;
					os << temppat->getBinaryString().toInt() << std::endl;
				} else {
					//default to raw binary
					os << temppat->getBinaryString() << std::endl;
				}
			}
			++it_patternList;
		}

	}
	return os;
}
std::ostream & operator<<(std::ostream & os, const PatternChannel & obj) {
	std::cout << "PatternChannel:: refid: " << obj.refID << " width: " << obj.width << " length: " << obj.length
			<< " maxPatternSize: " << obj.maxPatternListSize << " patternPosition: " << obj.patternPosition
			<< " patternMap: " << obj.patternMap.size() << std::endl;

	obj.printBinaryFormattedPatternList(os, true);
	return os;
}

} //NAMESPACE

} //NAMESPACE
