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

namespace cryomesh {

namespace state {

class PatternChannelMap : public common::KeyMappedCollection<boost::uuids::uuid,  PatternChannel >  {
public:
	PatternChannelMap(){}
	virtual ~PatternChannelMap(){}
};

}

}

#endif /* PATTERNCHANNELMAP_H_ */
