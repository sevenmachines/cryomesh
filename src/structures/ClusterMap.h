/*
 * ClusterMap.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#ifndef CLUSTERMAP_H_
#define CLUSTERMAP_H_

#include "common/KeyMappedCollection.h"
#include "Cluster.h"

namespace cryomesh {

namespace structures {

class ClusterMap: public common::KeyMappedCollection<boost::uuids::uuid, Cluster> {
public:
	ClusterMap() {
	}
	virtual ~ClusterMap() {
	}
	virtual void update() {
		std::cout<<"ClusterMap::update: "<<this<<std::endl;
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				it_objects->second->update();
				++it_objects;
			}
		}
	}

	friend std::ostream & operator<<(std::ostream & os, const ClusterMap & clustermap) {
		std::string tab = "";
		// clusters general
		const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters = clustermap.getCollection();

		// forall in all_clusters
		{
			os << tab << "ClusterMap:" << std::endl;
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();

			int count = 0;
			tab = "\t";
			while (it_all_clusters != it_all_clusters_end) {
				os << tab << *(it_all_clusters->second) << std::endl;
				++it_all_clusters;
			}
		}
		return os;
	}

};

}

}

#endif /* CLUSTERMAP_H_ */
