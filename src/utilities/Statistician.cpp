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
			inputChannelsCount(0), outputChannelsCount(0) {
	this->update();
}

Statistician::~Statistician() {
}

void Statistician::update() {
	std::cout<<"Statistician::update: "<<""<<std::endl;
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

std::ostream& operator<<(std::ostream & os, const Statistician & obj){
	os<<"Statistician::operator <<: "<<""<<std::endl;
	return os;
}


}//NAMESPACE

}//NAMESPACE
