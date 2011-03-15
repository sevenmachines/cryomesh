/*
 * FibreMap.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef FIBREMAP_H_
#define FIBREMAP_H_

#include "common/KeyMappedCollection.h"
#include "Fibre.h"

namespace cryomesh {

namespace structures {

class FibreMap: public common::KeyMappedCollection<boost::uuids::uuid, Fibre> {
public:
	FibreMap() {
	}
	virtual ~FibreMap() {
	}
	virtual void update() {
#ifdef FIBREMAP_DEBUG
		std::cout<<"FibreMap::update: "<<this<<std::endl;
#endif
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				it_objects->second->update();
				++it_objects;
			}
		}
	}

	friend std::ostream & operator<<(std::ostream & os, const FibreMap & objs) {
		std::string tab = "";

		const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> > & all_objs = objs.getCollection();

		// forall in all_objs
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_objs = all_objs.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Fibre> >::const_iterator it_all_objs_end =
					all_objs.end();
			while (it_all_objs != it_all_objs_end) {
				os << *(it_all_objs->second) << std::endl;
				++it_all_objs;
			}
		}
		return os;
	}
};

}

}

#endif /* FIBREMAP_H_ */
