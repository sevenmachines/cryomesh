/*
 * Statistician.cpp
 *
 *  Created on: 6 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#include "Statistician.h"
#include "structures/Bundle.h"

namespace cryomesh {

namespace utilities {

Statistician::Statistician(const structures::Bundle & bun) :
	bundle(bun), clusterCount(0), inputFibresCount(0), outputFibresCount(0), normalFibresCount(0),
			inputChannelsCount(0), outputChannelsCount(0) , bundleuuid(bun.getUUIDString()){
	this->update();
}

Statistician::~Statistician() {
}

void Statistician::update() {
	//std::cout << "Statistician::update: " << "" << std::endl;
	clusterCount = bundle.getClusters().getSize();
	inputFibresCount = bundle.getInputFibres().getSize();
	outputFibresCount = bundle.getOutputFibres().getSize();
	normalFibresCount = bundle.getFibres().getSize();
	inputChannelsCount = bundle.getInputChannelsMap().getSize();
	outputChannelsCount = bundle.getOutputChannelsMap().getSize();

}

int Statistician::getClusterCount() const {
	return clusterCount;
}
int Statistician::getInputFibresCount() const {
	return inputFibresCount;
}
int Statistician::getOutputFibresCount() const {
	return outputFibresCount;
}
int Statistician::getNormalFibresCount() const {
	return normalFibresCount;
}
int Statistician::getInputChannelsCount() const {
	return inputChannelsCount;
}
int Statistician::getOutputChannelsCount() const {
	return outputChannelsCount;
}
std::string Statistician::getBundleUUID()const{
	return bundleuuid;
}

std::map<std::string, int> Statistician::getTriggeredNodesPerCluster() const {
	std::map<std::string, int> activenodemap;
	const std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> > & clusters = bundle.getClusters().getCollection();
	// forall in clusters
		 {
			 std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> >::const_iterator it_clusters = clusters.begin();
			 const  std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> >::const_iterator it_clusters_end = clusters.end();
			 while ( it_clusters != it_clusters_end){
				 // get total active nodes
				 activenodemap[it_clusters->second->getUUIDString()] = it_clusters->second->getTriggeredNodeCount();
				 ++it_clusters;
			 }
		 }
		 return activenodemap;
}

std::map<std::string, int> Statistician::getActiveNodesPerCluster() const {
	std::map<std::string, int> activenodemap;
	const std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> > & clusters = bundle.getClusters().getCollection();
	// forall in clusters
		 {
			 std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> >::const_iterator it_clusters = clusters.begin();
			 const  std::map<boost::uuids::uuid, boost::shared_ptr<structures::Cluster> >::const_iterator it_clusters_end = clusters.end();
			 while ( it_clusters != it_clusters_end){
				 // get total active nodes
				 activenodemap[it_clusters->second->getUUIDString()] = it_clusters->second->getActiveNodeCount();
				 ++it_clusters;
			 }
		 }
		 return activenodemap;
}

int Statistician::getTriggeredNodesTotal() const {
	std::map<std::string, int> nodemap = this->getTriggeredNodesPerCluster();
	int total = 0;
	// forall in nodemap
	{
		std::map<std::string, int>::const_iterator it_nodemap = nodemap.begin();
		const std::map<std::string, int>::const_iterator it_nodemap_end = nodemap.end();
		while (it_nodemap != it_nodemap_end) {
			total += it_nodemap->second;
			++it_nodemap;
		}
	}
	return total;
}

int Statistician::getActiveNodesTotal() const {
	std::map<std::string, int> nodemap = this->getActiveNodesPerCluster();
	int total = 0;
	// forall in nodemap
	{
		std::map<std::string, int>::const_iterator it_nodemap = nodemap.begin();
		const std::map<std::string, int>::const_iterator it_nodemap_end = nodemap.end();
		while (it_nodemap != it_nodemap_end) {
			int cluster_total =  it_nodemap->second;
			//std::cout<<"Statistician::getActiveNodesTotal: "<<cluster_total<<std::endl;
			total +=cluster_total;
			++it_nodemap;
		}
	}
	return total;
}

const structures::Bundle & Statistician::getBundle()const{
	return bundle;
}

std::ostream& operator<<(std::ostream & os, const Statistician & obj) {
	os << "Statistician:: cycle: "<<common::TimeKeeper::getTimeKeeper().getCycle()<<" " << "bundle: clusters: "<<obj.getClusterCount()<<" id: "<<obj.getBundleUUID()<< std::endl;
	os <<"\tFibres: "<<"input: "<<obj.inputFibresCount <<" output: "<<obj.outputFibresCount<<" intermediate: "<<obj.normalFibresCount<<std::endl;
	os <<"\tChannels: "<<"input: "<<obj.inputChannelsCount <<" output: "<<obj.outputChannelsCount<<std::endl;
	os <<"Nodes: triggered: "<<obj.getTriggeredNodesTotal()<<" active: "<<obj.getActiveNodesTotal();

	return os;
}

}//NAMESPACE

}//NAMESPACE
