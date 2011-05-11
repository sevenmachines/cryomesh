/*
 * ClusterMap.h
 *
 *  Created on: 15 Feb 2011
 *      Author: niall
 */

#define CLUSTERMAP_DEBUG

#ifndef CLUSTERMAP_H_
#define CLUSTERMAP_H_

#include "common/KeyMappedCollection.h"
#include "Cluster.h"

namespace cryomesh {

namespace structures {

class ClusterMap: public common::KeyMappedCollection<boost::uuids::uuid, Cluster> {
public:

	enum EnergyFractionMethod {
		ENERGY_FRACTION_BY_CLUSTER_COUNT, ENERGY_FRACTION_BY_NODE_COUNT
	};
	ClusterMap() :
		totalNodeCount(0), energyFractionMethod(ENERGY_FRACTION_BY_NODE_COUNT) {
	}

	void setEnergyFractionMethod(EnergyFractionMethod method) {
		energyFractionMethod = method;
	}

	virtual ~ClusterMap() {
	}
	virtual void update() {
		totalNodeCount = 0;
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				boost::shared_ptr<Cluster> & current_cluster = it_objects->second;
				current_cluster->update();
				int current_node_count = current_cluster->getNodeMap().getSize();
				totalNodeCount += current_node_count;

				++it_objects;
			}
		}
	}

	void updateClusterEnergies(double total_energy) {
		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters = this->getMutableCollection();
		double energy_expended = 0;
		// forall in all_clusters
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();
			while (it_all_clusters != it_all_clusters_end) {
				double energy_fraction = 0;
				if (energyFractionMethod == ENERGY_FRACTION_BY_CLUSTER_COUNT) {
					energy_fraction = this->getClusterEnergyByClusterCount(it_all_clusters->second, total_energy);
				} else if (energyFractionMethod == ENERGY_FRACTION_BY_NODE_COUNT) {
					energy_fraction = this->getClusterEnergyByNodeCount(it_all_clusters->second, total_energy);
				} else {
					std::cout << "updateClusterEnergies: " << "ERROR: No energy method found" << std::endl;
					assert(false);
				}
				it_all_clusters->second->setEnergy(energy_fraction);
				energy_expended += energy_fraction;
				++it_all_clusters;
			}
		}
#ifdef CLUSTERMAP_DEBUG
		if (common::Maths::compareDoubles(energy_expended, total_energy) != 0) {
			std::cout << "updateClusterEnergies: " << "ERROR: Total Energy != Expended Energy" << std::endl;
			assert(false);
		}
#endif
	}

	/**
	 * Take a total energy and split it evenly between clusters
	 */
	double getClusterEnergyByClusterCount(const boost::shared_ptr<Cluster> & cluster, double total_energy) const {
		double node_fraction = 1.0 / (double) this->getCollection().size();
		double energy_fraction = total_energy * node_fraction;
		return energy_fraction;
	}

	/**
	 * Take a total energy and split it between clusters based on fraction of total nodes
	 */
	double getClusterEnergyByNodeCount(const boost::shared_ptr<Cluster> & cluster, double total_energy) const {
		int current_cluster_node_count = cluster->getNodeMap().getSize();
		double node_fraction = 0;
		if (totalNodeCount > 0) {
			node_fraction = (double) current_cluster_node_count / (double) this->totalNodeCount;
		}
		assert(node_fraction>0);
		assert(node_fraction<=1);
		double energy_fraction = total_energy * node_fraction;
		return energy_fraction;
	}

	void warpClusterMeshes() {

		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters = this->getMutableCollection();
		// forall in all_clusters
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();
			while (it_all_clusters != it_all_clusters_end) {
				it_all_clusters->second->warpMesh();
				++it_all_clusters;
			}
		}
	}

	void updateMapData() {
		totalNodeCount = 0;
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				boost::shared_ptr<Cluster> & current_cluster = it_objects->second;
				int current_node_count = current_cluster->getNodeMap().getSize();
				totalNodeCount += current_node_count;

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

private:
	int totalNodeCount;
	EnergyFractionMethod energyFractionMethod;
};

}

}

#endif /* CLUSTERMAP_H_ */
