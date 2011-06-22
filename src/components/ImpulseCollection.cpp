/**
 * ImpulseCollection.cpp
 *
 *  Created on: 20 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#include "ImpulseCollection.h"
#include "common/TimeKeeper.h"
#include "common/Maths.h"

namespace cryomesh {

namespace components {

ImpulseCollection::ImpulseCollection() {

}

ImpulseCollection::~ImpulseCollection() {
}

double ImpulseCollection::getActivity(common::Cycle cycle) const {
	const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > impulses = this->getCollection();
	double total_activity;
	if (this->objects.size() == 0) {
		total_activity = 0;
	} else {
		// forall in impulses
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses = impulses.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses_end =
					impulses.end();
			while (it_impulses != it_impulses_end) {
				double temp_act = it_impulses->second->getActivity(cycle);
				total_activity += temp_act;
				++it_impulses;
			}
		}
	}
	return total_activity;
}

double ImpulseCollection::getActivity() const {
	common::Cycle current_cycle = common::TimeKeeper::getTimeKeeper().getCycle();
	return this->getActivity(current_cycle);
}

void ImpulseCollection::clearImpulses() {
	common::Cycle current_cycle = common::TimeKeeper::getTimeKeeper().getCycle();
	this->clearImpulses(current_cycle);
}
void ImpulseCollection::clearImpulses(common::Cycle cycle) {
	this->clearImpulses(common::Cycle(), cycle);
}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearImpulses(common::Cycle cycleStart,
		common::Cycle cycleEnd) {
	//	std::cout << "ImpulseCollection::clearCycles: " <<"impulses:"<<this->getCollection().size()<< "(" << cycleStart << ", " << cycleEnd << ")" << std::endl;
	// switch cycles if start>end
	if (cycleStart > cycleEnd) {
		common::Cycle temp = cycleEnd;
		cycleEnd = cycleStart;
		cycleStart = temp;
	}
	// clear zeroed Impulses

	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > cleared_impulses;
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > &impulses = this->getMutableCollection();

	int impulse_pre_sz = impulses.size();

	// forall in impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses = impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses_end =
				impulses.end();
		while (it_impulses != it_impulses_end) {
			common::Cycle first_cycle = it_impulses->second->getFirstActiveCycle();
			common::Cycle second_cycle = it_impulses->second->getLastActiveCycle();
			//		std::cout<<"ImpulseCollection::clearImpulses: "<<"Checking: ["<<first_cycle.toLInt()<<", "<<second_cycle<<"]"<<std::endl;

			if ((first_cycle >= cycleStart && second_cycle < cycleEnd)) {
				//		std::cout<<"ImpulseCollection::clearImpulses: "<<"Deleting: ["<<first_cycle.toLInt()<<", "<<second_cycle<<"]"<<std::endl;
				cleared_impulses[it_impulses->first] = it_impulses->second;
			}
			++it_impulses;
		}
	}
	//std::cout<<"ImpulseCollection::clearImpulses: "<<"impulses:"<<impulses.size()<<" clear:"<<cleared_impulses.size()<<std::endl;
	// actually clear impulses
	// forall in cleared_impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses =
				cleared_impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses_end =
				cleared_impulses.end();
		while (it_cleared_impulses != it_cleared_impulses_end) {
			impulses.erase(it_cleared_impulses->first);
			++it_cleared_impulses;
		}
	}
	int impulse_post_sz = impulses.size();

	if (this->isDebugOn() == true) {
		if (impulse_pre_sz > 0) {
			std::cout << "ImpulseCollection::clearImpulses: " << "current: "
					<< common::TimeKeeper::getTimeKeeper().getCycle() << " range: " << "(" << cycleStart << ","
					<< cycleEnd << ") " << "deleted: " << impulse_pre_sz - impulse_post_sz << " " << *this << std::endl;
		}
	}
	return cleared_impulses;
}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActiveImpulses() {
	return (this->clearActiveImpulses(common::TimeKeeper::getTimeKeeper().getCycle()));

}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActiveImpulses(common::Cycle cycle) {
	return (this->clearActiveImpulses(cycle, cycle));
}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActiveImpulses(
		common::Cycle cycleStart, common::Cycle cycleEnd) {
	// switch cycles if start>end
	if (cycleStart > cycleEnd) {
		common::Cycle temp = cycleEnd;
		cycleEnd = cycleStart;
		cycleStart = temp;
	}

	// Impulses that are active
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > cleared_impulses;
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > &impulses = this->getMutableCollection();
	int impulse_pre_sz = impulses.size();

	// forall in impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses = impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses_end =
				impulses.end();
		while (it_impulses != it_impulses_end) {
			const Impulse & temp_impulse = *(it_impulses->second);
			common::Cycle first_cycle = temp_impulse.getFirstActiveCycle();
			common::Cycle second_cycle = temp_impulse.getLastActiveCycle();
			bool active = temp_impulse.isActive(cycleStart, cycleEnd);
			if (active == true) {
				cleared_impulses[it_impulses->first] = it_impulses->second;
			}
			++it_impulses;
		}
	}

	// actually clear impulses
	// forall in cleared_impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses =
				cleared_impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses_end =
				cleared_impulses.end();
		while (it_cleared_impulses != it_cleared_impulses_end) {
			impulses.erase(it_cleared_impulses->first);
			++it_cleared_impulses;
		}
	}
	int impulse_post_sz = impulses.size();

	if (this->isDebugOn() == true) {
		std::cout << "ImpulseCollection::clearActiveImpulses: " << "current: "
				<< common::TimeKeeper::getTimeKeeper().getCycle() << " range: " << "(" << cycleStart << "," << cycleEnd
				<< ") " << "deleted: " << impulse_pre_sz - impulse_post_sz << std::endl;
	}
	return cleared_impulses;
}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActivitiesByMinimum(double activity) {
	return this->clearActivitiesByValue(activity, false);
}

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActivitiesByMaximum(double activity) {
	return this->clearActivitiesByValue(activity, true);
}

void ImpulseCollection::decrementActivityTimers() {
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > &all_objs = this->getMutableCollection();
	// forall in all_objs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_objs = all_objs.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_objs_end =
				all_objs.end();
		while (it_all_objs != it_all_objs_end) {
			boost::shared_ptr<ActivityTimerDistance> temp_timer = (it_all_objs->second->getMutableActivityTimer());
			assert(temp_timer != 0);
			--(*temp_timer);
			++it_all_objs;
		}
	}
}

std::list<boost::shared_ptr<Impulse> > ImpulseCollection::getByActivityTimerValue(double value,
		ImpulseCollection::Comparison comp) {

	std::list < boost::shared_ptr<Impulse> > matched_impulses;
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > &all_objs = this->getMutableCollection();
	// forall in all_objs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_objs = all_objs.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_all_objs_end =
				all_objs.end();
		while (it_all_objs != it_all_objs_end) {
			double val = it_all_objs->second->getActivityTimer()->getDelay();

			bool match = false;
			switch (comp) {
			case (GreaterThan):
				match = val > value;
				break;

			case (LessThan):
				match = val < value;
				break;

			case (EqualTo):
				match = common::Maths::compareDoubles(val, value) == 0;
				break;

			case (GreaterThanOrEqualTo):
				match = val > value || common::Maths::compareDoubles(val, value) == 0;
				break;

			case (LessThanOrEqualTo):
				match = val < value || common::Maths::compareDoubles(val, value) == 0;
				break;

			default:
				match = false;
				assert(false);
				break;
			}

			if (match == true) {
				matched_impulses.push_back(it_all_objs->second);
			}
			++it_all_objs;
		}
	}
	return matched_impulses;
}

std::list<boost::shared_ptr<Impulse> > ImpulseCollection::removeByActivityTimerValue(double value,
		ImpulseCollection::Comparison comp) {
	std::list < boost::shared_ptr<Impulse> > matched_impulses = this->getByActivityTimerValue(value, comp);
	this->remove(matched_impulses);
	return matched_impulses;
}

void ImpulseCollection::refreshDataObject() {
	//	std::cout<<"ImpulseCollection::refreshDataObject: "<<""<<std::endl;
	if (dataObject.isLoggingEnabled() == true) {
		dataObject.clear();

		// end cycle
		common::Cycle end_cycle = common::TimeKeeper::getTimeKeeper().getCycle();
		// start cycle
		int cycle_diff = end_cycle.toULInt() - dataObject.getDatasetMaximumSize();

		common::Cycle start_cycle(std::max(0, cycle_diff));

		//	std::cout<<"ImpulseCollection::refreshDataObject: "<<"start:"<<start_cycle<<" end:"<<end_cycle<<std::endl;
		for (common::Cycle count = start_cycle; count < end_cycle; count++) {
			//	std::cout<<"ImpulseCollection::refreshDataObject: "<<"count:"<<count<<std::endl;
			dataObject.insert(count.toULInt(), this->getActivity(count));
		}
	}
}

ImpulseCollection & ImpulseCollection::operator=(const ImpulseCollection & obj) {
	if (*this == obj) {
		return *this;
	}

	this->objects = std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >(obj.getCollection());
	return *this;
}

ImpulseCollection & ImpulseCollection::operator+=(const ImpulseCollection & obj) {
	const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > & collection_obj = obj.getCollection();

	// forall in collection_this
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_obj =
				collection_obj.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_obj_end =
				collection_obj.end();

		while (it_collection_obj != it_collection_obj_end) {
			boost::shared_ptr<Impulse> temp_impulse(it_collection_obj->second);
			this->objects.insert(
					std::pair<boost::uuids::uuid, boost::shared_ptr<Impulse>>(temp_impulse->getUUID(), temp_impulse));
			++it_collection_obj;
		}
	}
	return *this;
}

const ImpulseCollection ImpulseCollection::operator+(const ImpulseCollection & obj) const {
	ImpulseCollection imp = *this;
	imp += obj;
	return imp;
}

void ImpulseCollection::enableDebug(bool b) {
	this->setDebug(b);
}

bool ImpulseCollection::operator==(const ImpulseCollection & obj) const {
	// compare mesh
	/*{
	 bool equality = this->getMesh() == obj.getMesh();
	 if (equality == false) {
	 return equality;
	 }
	 }*/

	// compare activities
	{
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > & collection_this = this->getCollection();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > & collection_obj = obj.getCollection();

		// check size
		if (collection_this.size() != collection_obj.size()) {
			return false;
		}

		// forall in collection_this
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_this =
					collection_this.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_this_end =
					collection_this.end();
			std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_obj =
					collection_obj.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_collection_obj_end =
					collection_obj.end();

			while (it_collection_this != it_collection_this_end && it_collection_obj != it_collection_obj_end) {
				Impulse & temp1 = *(it_collection_this->second);
				Impulse & temp2 = *(it_collection_obj->second);
				bool eq = (temp1 == temp2);
				if (temp1 != temp2) {
					return false;
				}
				++it_collection_this;
				++it_collection_obj;
			}
		}

	}

	// IGNORED: uuid
	return true;
}

bool ImpulseCollection::operator!=(const ImpulseCollection &obj) const {
	return !(*this == obj);
}

std::ostream& operator<<(std::ostream & os, const ImpulseCollection & obj) {
	os << "ImpulseCollection: " << " size: " << obj.getCollection().size() << std::endl;

	const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > & allobjs = obj.getCollection();

	// forall in allobjs
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_allobjs = allobjs.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_allobjs_end = allobjs.end();
		while (it_allobjs != it_allobjs_end) {
			os << *(it_allobjs->second) << std::endl;
			++it_allobjs;
		}
	}

	return os;
}

// GETTERS/SETTERS

/*
 const boost::shared_ptr<Mesh> ImpulseCollection::getMesh() const {
 return mesh;
 }*/

std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> > ImpulseCollection::clearActivitiesByValue(double activity,
		bool greater) {
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > cleared_impulses;
	std::map < boost::uuids::uuid, boost::shared_ptr<Impulse> > &impulses = this->getMutableCollection();
	// forall in impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses = impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_impulses_end =
				impulses.end();
		while (it_impulses != it_impulses_end) {
			if (greater == true) {
				double act = it_impulses->second->getActivityMaximum();
				if (act > activity) {
					cleared_impulses[it_impulses->first] = it_impulses->second;
				}
			} else if (greater == false) {
				double act = it_impulses->second->getActivityMinimum();
				if (act < activity) {
					cleared_impulses[it_impulses->first] = it_impulses->second;
				}
			}
			++it_impulses;
		}
	}

	// actually clear impulses
	// forall in cleared_impulses
	{
		std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses =
				cleared_impulses.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Impulse> >::const_iterator it_cleared_impulses_end =
				cleared_impulses.end();
		while (it_cleared_impulses != it_cleared_impulses_end) {
			impulses.erase(it_cleared_impulses->first);
			++it_cleared_impulses;
		}
	}
	return cleared_impulses;
}
} //NAMESPACE

} //NAMESPACE
