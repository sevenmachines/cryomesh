#include "NodeDatabaseObject.h"

namespace cryomesh {

namespace manager {
const std::string NodeDatabaseObject::ID_TAG = "id";
const std::string NodeDatabaseObject::X_TAG = "x";
const std::string NodeDatabaseObject::Y_TAG = "y";
const std::string NodeDatabaseObject::Z_TAG = "z";
const std::string NodeDatabaseObject::ACTIVITY_TAG = "activity";
const std::string NodeDatabaseObject::CYCLE_TAG = "cycle";

NodeDatabaseObject::NodeDatabaseObject(std::string uuid_str, spacial::Point pt, common::Cycle cyc, double act) : uuid(uuid_str), point(pt), cycle(cyc), activity(act){
	columns[ID_TAG] = uuid_str;
	columns[X_TAG] = toString<double> (point.getX());
	columns[Y_TAG] = toString<double> (point.getY());
	columns[Z_TAG] = toString<double> (point.getZ());
	columns[CYCLE_TAG] = toString<unsigned long int> (cycle.toULInt());
	columns[ACTIVITY_TAG] = toString<double> (activity);
}

NodeDatabaseObject::NodeDatabaseObject(const std::string & node_table_entry){
	//TODO NodeDatabaseObject::NodeDatabaseObject(const std::string & node_table_entry)
	std::cout<<"TODO NodeDatabaseObject::NodeDatabaseObject(const std::string & node_table_entry)"<<std::endl;
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

 std::string NodeDatabaseObject::getUUIDString() const{
	 return uuid;
}
const  spacial::Point & NodeDatabaseObject::getPoint() const{
	 return point;
}
 const common::Cycle & NodeDatabaseObject::getCycle() const{
	 return cycle;
}
 double NodeDatabaseObject::getActivity() const{
	 return activity;
}

}//NAMESPACE

}//NAMESPACE
