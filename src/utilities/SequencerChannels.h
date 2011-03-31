/*
 * SequencerChannels.h
 *
 *  Created on: 1 Sep 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef SEQUENCERCHANNELS_H_
#define SEQUENCERCHANNELS_H_

#include "state/PatternChannel.h"
#include "state/Pattern.h"
#include "state/PatternChannelMap.h"

#include "SequencerGeneric.h"
#include <map>
#include <boost/uuid/uuid.hpp>
namespace cryomesh {
namespace utilities {

class SequencerChannels {
public:
	SequencerChannels();
	virtual ~SequencerChannels();

	/*
	 * read in a set of SequenceChannels from a file
	 */
	void readSequences(const std::string & ifstr);
	/*
	 * write out a set of sequences to a file
	 */
	void writeSequences(const std::string & ofstr,
			std::map<boost::uuids::uuid, boost::shared_ptr<
					state::PatternChannel> > & in_channels,
			std::map<boost::uuids::uuid, boost::shared_ptr<
					state::PatternChannel> > & out_channels) const;

	 std::map<boost::uuids::uuid, boost::shared_ptr<
				state::PatternChannel> >  getInputChannels();
	 std::map<boost::uuids::uuid, boost::shared_ptr<
					state::PatternChannel> >  getOutputChannels();

	 state::PatternChannelMap  getInputChannelsMap() const;
	 state::PatternChannelMap  getOutputChannelsMap() const;

	//statics
	static const std::string PATTERN_CHANNEL_STRING;
	static const std::string PATTERN_CHANNEL_TYPE_STRING;
	static const std::string PATTERN_CHANNEL_INPUT_STRING;
	static const std::string PATTERN_CHANNEL_OUTPUT_STRING;
	static const std::string PATTERN_CHANNEL_REFID_STRING;
	static const std::string PATTERN_STRING;
	static const std::string PATTERN_BINARY_STRING;
	static const std::string PATTERN_TAG_STRING;
	static const std::string VERSION_STRING;
	static const std::string DESCRIPTION_STRING;
	static const std::string PATTERN_CHANNEL_WIDTH_STRING;
	static const std::string PATTERN_CHANNEL_DEPTH_STRING;
	static const std::string PATTERN_CHANNEL_NOTE_STRING;

protected:
	state::PatternChannelMap getChannelMap(const std::map<boost::uuids::uuid, boost::shared_ptr<
					state::PatternChannel> > & patchans) const;
private:
	std::map<boost::uuids::uuid, boost::shared_ptr<
			state::PatternChannel> > in_channels;
	std::map<boost::uuids::uuid, boost::shared_ptr<
			state::PatternChannel> > out_channels;
	std::map<boost::uuids::uuid, boost::shared_ptr<
				state::PatternChannel> > in_channels_filtered;
		std::map<boost::uuids::uuid, boost::shared_ptr<
				state::PatternChannel> > out_channels_filtered;
};

}

}


#endif /* SEQUENCERCHANNELS_H_ */
