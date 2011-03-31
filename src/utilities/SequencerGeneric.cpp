#include "SequencerGeneric.h"
#include <glibmm/convert.h> //For Glib::ConvertError
#include <iostream>
namespace cryomesh {
namespace utilities {

SequencerGeneric::SequencerGeneric(const std::string & ifstr) :
	xmlpp::SaxParser() {
	std::ifstream ifs(ifstr);
	if (ifs.is_open() !=true){
		std::cout<<"SequencerGeneric::SequencerGeneric: "<<"ERROR: Cannot open file "<<"'"<<ifstr<<"'"<<std::endl;
	}else{
		ifs.close();
		this->parse_file(ifstr);
}
}

SequencerGeneric::~SequencerGeneric() {
}

void SequencerGeneric::on_start_document() {
	//std::cout << "on_start_document()" << std::endl;
}

void SequencerGeneric::on_end_document() {
	//std::cout << "on_end_document()" << std::endl;
}

void SequencerGeneric::on_start_element(const Glib::ustring& name,
		const AttributeList& attributes) {
	boost::shared_ptr<NodeEntry> node_entry(new NodeEntry);
	if (nodeStack.size() > 0) {
		node_entry->parentNode = nodeStack.back();
		nodeStack.back()->childNodes.push_back(node_entry);
	}
	nodeStack.push_back(node_entry);

	//std::cout << "node name=" << name << std::endl;
	node_entry->name = name;
	// Print attributes:

	std::string attrib_name;
	std::string attrib_value;
	for (xmlpp::SaxParser::AttributeList::const_iterator iter =
			attributes.begin(); iter != attributes.end(); ++iter) {
		try {
			//std::cout << "  Attribute name=" << iter->name << std::endl;
			attrib_name = iter->name;
		} catch (const Glib::ConvertError& ex) {
			std::cerr
					<< "SequencerGeneric::on_start_element(): Exception caught while converting name for std::cout: "
					<< ex.what() << std::endl;
		}

		try {
		//	std::cout << "    , value= " << iter->value << std::endl;
			attrib_value = iter->value;
		} catch (const Glib::ConvertError& ex) {
			std::cerr
					<< "SequencerGeneric::on_start_element(): Exception caught while converting value for std::cout: "
					<< ex.what() << std::endl;
		}
		node_entry->info[attrib_name] = attrib_value;
	}
	nodeEntries.push_back(node_entry);
}

void SequencerGeneric::on_end_element(const Glib::ustring& /* name */) {
	nodeStack.pop_back();
	//std::cout << "on_end_element()" << std::endl;
}

void SequencerGeneric::on_characters(const Glib::ustring& text) {
	try {
		//std::cout << "on_characters(): " << text << std::endl;
	} catch (const Glib::ConvertError& ex) {
		std::cerr
				<< "SequencerGeneric::on_characters(): Exception caught while converting text for std::cout: "
				<< ex.what() << std::endl;
	}
}

void SequencerGeneric::on_comment(const Glib::ustring& text) {
	try {
		//std::cout << "on_comment(): " << text << std::endl;
	} catch (const Glib::ConvertError& ex) {
		std::cerr
				<< "SequencerGeneric::on_comment(): Exception caught while converting text for std::cout: "
				<< ex.what() << std::endl;
	}
}

void SequencerGeneric::on_warning(const Glib::ustring& text) {
	try {
	//	std::cout << "on_warning(): " << text << std::endl;
	} catch (const Glib::ConvertError& ex) {
		std::cerr
				<< "SequencerGeneric::on_warning(): Exception caught while converting text for std::cout: "
				<< ex.what() << std::endl;
	}
}

void SequencerGeneric::on_error(const Glib::ustring& text) {
	try {
		//std::cout << "on_error(): " << text << std::endl;
	} catch (const Glib::ConvertError& ex) {
		std::cerr
				<< "SequencerGeneric::on_error(): Exception caught while converting text for std::cout: "
				<< ex.what() << std::endl;
	}
}

void SequencerGeneric::on_fatal_error(const Glib::ustring& text) {
	try {
		//std::cout << "on_fatal_error(): " << text << std::endl;
	} catch (const Glib::ConvertError& ex) {
		std::cerr
				<< "SequencerGeneric::on_characters(): Exception caught while converting value for std::cout: "
				<< ex.what() << std::endl;
	}
}

const std::list< boost::shared_ptr<SequencerGeneric::NodeEntry> > & SequencerGeneric::getNodeEntries() const{
		return nodeEntries;
}
std::ostream & operator<<(std::ostream& os, const SequencerGeneric & obj) {
	//for all in nodeEntries
	{
		std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator
				it_nodeEntries = obj.nodeEntries.begin();
		const std::list<boost::shared_ptr<SequencerGeneric::NodeEntry> >::const_iterator
				it_nodeEntries_end = obj.nodeEntries.end();
		while (it_nodeEntries != it_nodeEntries_end) {
			os << **it_nodeEntries << std::endl;
			++it_nodeEntries;
		}

	}
	return os;
}

}//NAMESPACE
}//NAMESPACE
