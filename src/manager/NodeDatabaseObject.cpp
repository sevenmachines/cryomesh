#include "NodeDatabaseObject.h"

namespace cryomesh {

namespace manager {
const std::string NodeDatabaseObject::ID_TAG = "id";
const std::string NodeDatabaseObject::X_TAG = "x";
const std::string NodeDatabaseObject::Y_TAG = "y";
const std::string NodeDatabaseObject::Z_TAG = "z";
const std::string NodeDatabaseObject::ACTIVITY_TAG = "activity";
const std::string NodeDatabaseObject::CYCLE_TAG = "cycle";

NodeDatabaseObject::NodeDatabaseObject(std::string uuid_str, spacial::Point pt, common::Cycle cyc, double act) :
	uuid(uuid_str), point(pt), cycle(cyc), activity(act) {
	columns[ID_TAG] = uuid_str;
	columns[X_TAG] = toString<double> (point.getX());
	columns[Y_TAG] = toString<double> (point.getY());
	columns[Z_TAG] = toString<double> (point.getZ());
	columns[CYCLE_TAG] = toString<unsigned long int> (cycle.toULInt());
	columns[ACTIVITY_TAG] = toString<double> (activity);
}

NodeDatabaseObject::NodeDatabaseObject(const std::string & node_table_entry) :uuid(), point(0,0,0),  cycle(), activity(0){
	//std::cout << "TODO NodeDatabaseObject::NodeDatabaseObject(const std::string & node_table_entry)" << std::endl;

	std::map<std::string, std::string> entry_map = DatabaseObject::getColumnMapFromEntry(node_table_entry);
	std::string uuid_str = DatabaseObject::findValue("id", entry_map);
	std::string x_str = DatabaseObject::findValue("x", entry_map);
	std::string y_str = DatabaseObject::findValue("y", entry_map);
	std::string z_str = DatabaseObject::findValue("z", entry_map);
	std::string activity_str = DatabaseObject::findValue("activity", entry_map);
	std::string cycle_str = DatabaseObject::findValue("cycle", entry_map);

	this->uuid = uuid_str;
	double x = (x_str == "") ? 0 : atof(x_str.c_str());
	double y = (y_str == "") ? 0 : atof(y_str.c_str());
	double z = (z_str == "") ? 0 : atof(z_str.c_str());
	this->point = spacial::Point(x, y, z);
	this->activity = (activity_str == "") ? 0 : atof(activity_str.c_str());
	long int cycleint = (cycle_str == "") ? 0 : atol(cycle_str.c_str());
	this->cycle = common::Cycle(cycleint);

	/*
	std::cout << "NodeDatabaseObject::NodeDatabaseObject: " << "FOUND:" << std::endl;
	std::cout << "\t" << "uuid_str" << "=" << uuid_str << std::endl;
	std::cout << "\t" << "x_str" << "=" << x_str << std::endl;
	std::cout << "\t" << "y_str" << "=" << y_str << std::endl;
	std::cout << "\t" << "z_str" << "=" << z_str << std::endl;
	std::cout << "\t" << "cycle_str" << "=" << cycle_str << std::endl;
	std::cout << "\t" << "activity_str" << "=" << activity_str << std::endl;

	std::cout << "NodeDatabaseObject::NodeDatabaseObject: " << "FOUND:" << std::endl;
	std::cout << "\t" << "uuid" << "=" << uuid << std::endl;
	std::cout << "\t" << "point" << "=" << point << std::endl;
	std::cout << "\t" << "cycle" << "=" << cycle << std::endl;
	std::cout << "\t" << "activity" << "=" << activity << std::endl;
*/
}

NodeDatabaseObject::~NodeDatabaseObject() {
}

std::string NodeDatabaseObject::getInsert(const std::string & table) const {
	//"insert into nodesTable (id, x, y, activity) values ('1', 0.5, 0.6, 4.5)"
	std::stringstream ss;
	ss << "insert into " << table << " (" << ID_TAG << ", " << X_TAG << ", " << Y_TAG << ", " << Z_TAG << ", "
			<< CYCLE_TAG << ", " << ACTIVITY_TAG << ") ";
	ss << " values (" << "\'" << getKey(ID_TAG) << "\'" << ", " << getKey(X_TAG) << ", " << getKey(Y_TAG) << ", "
			<< getKey(Z_TAG) << ", " << getKey(CYCLE_TAG) << ", " << getKey(ACTIVITY_TAG) << ");";
	return ss.str();

}

std::string NodeDatabaseObject::getUUID() const {
	return uuid;
}
const spacial::Point & NodeDatabaseObject::getPoint() const {
	return point;
}
const common::Cycle & NodeDatabaseObject::getCycle() const {
	return cycle;
}
double NodeDatabaseObject::getActivity() const {
	return activity;
}

}//NAMESPACE

}//NAMESPACE
