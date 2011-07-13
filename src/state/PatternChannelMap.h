/*
 * PatternChannelMap.h
 *
 *  Created on: 31 Mar 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef PATTERNCHANNELMAP_H_
#define PATTERNCHANNELMAP_H_

#include "PatternChannel.h"
#include "common/KeyMappedCollection.h"
#include "common/TimeKeeper.h"

namespace cryomesh {

namespace state {

class PatternChannelMap: public common::KeyMappedCollection<boost::uuids::uuid, PatternChannel> {
public:
	PatternChannelMap() {
	}
	virtual ~PatternChannelMap() {
	}

	/**
	 * Get a mapping of pattern channel uuids to their patterns on a specific cycle
	 *
	 */
	const std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> > getPatterns(
			const common::Cycle & cycle = common::TimeKeeper::getTimeKeeper().getTimeKeeper().getCycle()) {
		std::map<boost::uuids::uuid, boost::shared_ptr<state::Pattern> > found_patterns;

		const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > & all_patternchannels =
				this->getCollection();
		// forall in all_patternchannels
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
					it_all_patternchannels = all_patternchannels.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> >::const_iterator
					it_all_patternchannels_end = all_patternchannels.end();
			while (it_all_patternchannels != it_all_patternchannels_end) {
				found_patterns[it_all_patternchannels->first]
						= it_all_patternchannels->second->getPatternByCycle(cycle);
				++it_all_patternchannels;
			}
		}
		return found_patterns;
	}
};

}

}

#endif /* PATTERNCHANNELMAP_H_ */
