/*
 * SequencerChannels.cpp
 *
 *  Created on: 1 Sep 2010
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */
//#define SEQUENCERCHANNELS_DEBUG

#include "SequencerChannels.h"

namespace cryomesh {
namespace utilities {

using namespace cryomesh::state;

const std::string SequencerChannels::PATTERN_CHANNEL_STRING = "PatternChannel";
const std::string SequencerChannels::VERSION_STRING = "Version";
const std::string SequencerChannels::DESCRIPTION_STRING = "Description";

const std::string SequencerChannels::PATTERN_STRING = "Pattern";
const std::string SequencerChannels::PATTERN_BINARY_STRING = "Binary";
const std::string SequencerChannels::PATTERN_TAG_STRING = "Tag";
const std::string SequencerChannels::PATTERN_CHANNEL_TYPE_STRING = "Type";
const std::string SequencerChannels::PATTERN_CHANNEL_REFID_STRING = "ReferenceID";

const std::string SequencerChannels::PATTERN_CHANNEL_INPUT_STRING = "Input";
const std::string SequencerChannels::PATTERN_CHANNEL_OUTPUT_STRING = "Output";
const std::string SequencerChannels::PATTERN_CHANNEL_WIDTH_STRING = "Width";
const std::string SequencerChannels::PATTERN_CHANNEL_DEPTH_STRING = "Depth";
const std::string SequencerChannels::PATTERN_CHANNEL_NOTE_STRING = "Note";

SequencerChannels::SequencerChannels() {

}

SequencerChannels::~SequencerChannels() {
}

void SequencerChannels::readSequences(const std::string & ifstr) {
#ifdef SEQUENCERCHANNELS_DEBUG
	std::cout << "SequencerChannels::readSequences: " << ifstr << std::endl;
#endif
	in_channels.clear();
	out_channels.clear();
	SequencerGeneric sg(ifstr);
	const std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> > & node_entries = sg.getNodeEntries();
	//try to process tree
	//for all in node_entries
	{
		std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator it_node_entries =
				node_entries.begin();
		const std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator it_node_entries_end =
				node_entries.end();
		while (it_node_entries != it_node_entries_end) {

			SequencerGeneric::NodeEntry & node = (**it_node_entries);

			// check for PatternChannel tag and node present
			if ((node.name == PATTERN_CHANNEL_STRING) && node.childNodes.size() > 0) {
				std::list<boost::shared_ptr<Pattern> > pats;
				const std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> > &pat_nodes = node.childNodes;
				//for all in pat_nodes
				{
					std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator it_pat_nodes =
							pat_nodes.begin();
					const std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator it_pat_nodes_end =
							pat_nodes.end();
					while (it_pat_nodes != it_pat_nodes_end) {
						// create the patterns associated with this
						//TODO do something about PatternByTag
						std::map<std::string, std::string>::const_iterator it_found = (*it_pat_nodes)->info.find(
								PATTERN_BINARY_STRING);

						std::string pat_string;
						if (it_found != (*it_pat_nodes)->info.end()) {
							pat_string = it_found->second;
							boost::shared_ptr<Pattern> temp_pat(new Pattern(pat_string));
							pats.push_back(temp_pat);
						} else {
							std::cout << "SequencerChannels::readSequences: " << "WARNING: Pattern string not found "
									<< std::endl;
						}
						++it_pat_nodes;
					}
				}

				//collected all pattern nodes so now create the pattern channel

				// get refid
				std::map<std::string, std::string>::const_iterator it_refid = node.info.find(
						PATTERN_CHANNEL_REFID_STRING);
				unsigned int refid = atoi((it_refid->second).c_str());
				std::map<std::string, std::string>::const_iterator it_type =
						node.info.find(PATTERN_CHANNEL_TYPE_STRING);

				std::string type;
				if (it_type != node.info.end()) {
					type = it_type->second;
					if (type == PATTERN_CHANNEL_INPUT_STRING) {
						boost::shared_ptr<PatternChannel> temp_pch(new PatternChannel(pats, PatternChannel::Input));
						temp_pch->setRefID(refid);
						in_channels[temp_pch->getUUID()] = temp_pch;
					} else if (type == PATTERN_CHANNEL_OUTPUT_STRING) {
						boost::shared_ptr<PatternChannel> temp_pch(new PatternChannel(pats, PatternChannel::Output));
						temp_pch->setRefID(refid);
						out_channels[temp_pch->getUUID()] = temp_pch;
					}
				} else {
					std::cout << "SequencerChannels::readSequences: " << "ERROR: Channel type not found " << std::endl;
				}

			} else {
#ifdef SEQUENCERCHANNELS_DEBUG
				std::cout << "SequencerChannels::readSequences: " << "INFO: Skipping " << node.name << std::endl;
#endif
			}

			// save to inputs or outputs
			++it_node_entries;
		}
	}

}

std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > SequencerChannels::getInputChannels() {
	return in_channels;
}
std::map<boost::uuids::uuid, boost::shared_ptr<state::PatternChannel> > SequencerChannels::getOutputChannels() {
	return out_channels;
}

} //NAMESPACE


} //NAMESPACE
