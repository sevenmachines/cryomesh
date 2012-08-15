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

	ClusterMap() :
			totalNodeCount(0) {
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
#ifdef CLUSTERMAP_DEBUG
		double energy_expended = 0;
#endif
		// forall in all_clusters
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();

			double energy_fraction = 0;

			while (it_all_clusters != it_all_clusters_end) {

				if (it_all_clusters->second->getEnergyFractionMethod() == Cluster::ENERGY_FRACTION_BY_NODE_COUNT) {
					energy_fraction = this->getClusterEnergyByNodeCount(it_all_clusters->second, total_energy);
				} else if (it_all_clusters->second->getEnergyFractionMethod()
						== Cluster::ENERGY_FRACTION_BY_CLUSTER_COUNT) {
					energy_fraction = this->getClusterEnergyByClusterCount(total_energy);
				} else if (it_all_clusters->second->getEnergyFractionMethod() == Cluster::ENERGY_FRACTION_NULL) {
					std::cout << "updateClusterEnergies: " << "ERROR: No energy method found" << std::endl;
					assert(false);
				}
				it_all_clusters->second->setEnergy(energy_fraction);
				++it_all_clusters;
#ifdef CLUSTERMAP_DEBUG
				energy_expended += energy_fraction;
#endif
			}
		}
#ifdef CLUSTERMAP_DEBUG
		if (common::Maths::compareDoubles(energy_expended, total_energy) != 0) {
			std::cout << "updateClusterEnergies: " << "ERROR: Total Energy != Expended Energy" << std::endl;
			assert(false);
		}
#endif
	}

	virtual int getTotalNodeCount() const {
		int total_node_count = 0;
		// forall in objects
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_objects = objects.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_objects_end =
					objects.end();
			while (it_objects != it_objects_end) {
				int current_node_count = it_objects->second->getNodeMap().getSize();
				total_node_count += current_node_count;
				++it_objects;
			}
		}
		return total_node_count;
	}
	void updataEnergyFractionMethods(Cluster::EnergyFractionMethod method) {
		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
				this->objects.begin();
		const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
				this->objects.end();
		while (it_all_clusters != it_all_clusters_end) {

			if (method == Cluster::ENERGY_FRACTION_BY_NODE_COUNT) {
				totalNodeCount = this->getTotalNodeCount();

				double max_energy_fraction = static_cast<double>(it_all_clusters->second->getNodeMap().getSize())
						/ static_cast<double>(this->totalNodeCount);
				it_all_clusters->second->setEnergyFractionMethod(method, max_energy_fraction);

			} else if (method == Cluster::ENERGY_FRACTION_BY_CLUSTER_COUNT) {
				double max_energy_fraction = 1.0 / static_cast<double>(this->getSize());
				it_all_clusters->second->setEnergyFractionMethod(method, max_energy_fraction);
			} else if (method == Cluster::ENERGY_FRACTION_NULL) {
				std::cout << "updateClusterEnergies: " << "ERROR: No energy method found" << std::endl;
				assert(false);
			}
			++it_all_clusters;
		}
	}

	/**
	 * Take a total energy and split it evenly between clusters
	 */
	double getClusterEnergyByClusterCount(double total_energy) const {
		double node_fraction = 1.0 / static_cast<double>(this->getCollection().size());
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
			node_fraction = static_cast<double>(current_cluster_node_count) / static_cast<double>(this->totalNodeCount);
		}
		assert(node_fraction > 0);
		assert(node_fraction <= 1);
		double energy_fraction = total_energy * node_fraction;
		return energy_fraction;
	}

	void warpClusterNodes() {

		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters = this->getMutableCollection();
		// forall in all_clusters
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();
			while (it_all_clusters != it_all_clusters_end) {
				it_all_clusters->second->warpNodes();
				++it_all_clusters;
			}
		}
	}
	void runAnalysers() {

		std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> > & all_clusters = this->getMutableCollection();
		// forall in all_clusters
		{
			std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters =
					all_clusters.begin();
			const std::map<boost::uuids::uuid, boost::shared_ptr<Cluster> >::const_iterator it_all_clusters_end =
					all_clusters.end();
			while (it_all_clusters != it_all_clusters_end) {
				it_all_clusters->second->runAnalyser();
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
}
;

}

}

#endif /* CLUSTERMAP_H_ */
