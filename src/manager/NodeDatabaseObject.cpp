#include "NodeDatabaseObject.h"

namespace cryomesh {

namespace manager {
const std::string NodeDatabaseObject::ID_TAG = "id";
const std::string NodeDatabaseObject::X_TAG = "x";
const std::string NodeDatabaseObject::Y_TAG = "y";
const std::string NodeDatabaseObject::Z_TAG = "z";
const std::string NodeDatabaseObject::ACTIVITY_TAG = "activity";
const std::string NodeDatabaseObject::CYCLE_TAG = "cycle";

NodeDatabaseObject::NodeDatabaseObject(std::string uuid_str, spacial::Point point, unsigned long int cycle,
		double activity) {
	columns[ID_TAG] = uuid_str;
	columns[X_TAG] = toString<double> (point.getX());
	columns[Y_TAG] = toString<double> (point.getY());
	columns[Z_TAG] = toString<double> (point.getZ());
	columns[CYCLE_TAG] = toString<int> (cycle);
	columns[ACTIVITY_TAG] = toString<int> (activity);
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

}//NAMESPACE

}//NAMESPACE
