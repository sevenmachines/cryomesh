#ifndef SEQUENCERGENERIC_H_
#define SEQUENCERGENERIC_H_
#include <map>
#include <list>
#include <libxml++-2.6/libxml++/libxml++.h>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>
namespace cryomesh {
namespace utilities {

class SequencerGeneric: public xmlpp::SaxParser {
public:
	struct NodeEntry {
	public:
		std::string name;
		// map of string entry names to values held by the node
		std::map<std::string, std::string> info;
		boost::shared_ptr<NodeEntry> parentNode;
		std::list<boost::shared_ptr<NodeEntry> > childNodes;

		friend std::ostream & operator<<(std::ostream& os, const NodeEntry & obj){
			os<<"NodeEntry: "<<obj.name << " child: "<< obj.childNodes.size()<<std::endl;
			//for all in info
			{
				std::map < std::string , std::string  >::const_iterator it_info = obj.info.begin();
				const std::map < std::string , std::string  >::const_iterator it_info_end = obj.info.end();
				while(it_info!=it_info_end) {
					os<<"    "<<it_info->first<<": "<<it_info->second<<std::endl;
					++it_info;
				}
			}
			return os;
		}
	};

		SequencerGeneric(const std::string & ifstr);
		virtual ~SequencerGeneric();
		const std::list< boost::shared_ptr<SequencerGeneric::NodeEntry> > & getNodeEntries() const;

		virtual void on_start_document();
		virtual void on_end_document();
		virtual void on_start_element(const Glib::ustring& name,
				const AttributeList& properties);
		virtual void on_end_element(const Glib::ustring& name);
		virtual void on_characters(const Glib::ustring& characters);
		virtual void on_comment(const Glib::ustring& text);
		virtual void on_warning(const Glib::ustring& text);
		virtual void on_error(const Glib::ustring& text);
		virtual void on_fatal_error(const Glib::ustring& text);

		friend std::ostream & operator<<(std::ostream& os, const SequencerGeneric & obj);
	private:
		std::list< boost::shared_ptr<NodeEntry> > nodeEntries;
		std::list < boost::shared_ptr<NodeEntry> > nodeStack;
		int elementCount;
	};

}

}


#endif /* SEQUENCERGENERIC_H_ */
