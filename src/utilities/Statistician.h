/*
 * Statistician.h
 *
 *  Created on: 6 Apr 2011
 *      Author: "SevenMachines <SevenMachines@yahoo.co.uk>"
 */

#ifndef STATISTICIAN_H_
#define STATISTICIAN_H_

#include <iostream>

namespace cryomesh {

// Forward declare Bundle
namespace structures {
class Bundle;
}

namespace utilities {

/**
 * Class to draw together lots of useful statistics and monitoring data for a Bundle
 * and its components
 */
class Statistician {
public:
	/**
	 * Contruct a statistician with a build in Bundle reference
	 */
	Statistician(const structures::Bundle & bun);

	/**
	 * Default destructor
	 */
	virtual ~Statistician();

	void update();

	int getClusterCount() const;
	int getInputFibresCount() const;
	int getOutputFibresCount() const;
	int getNormalFibresCount() const;
	int getInputChannelsCount() const;
	int getOutputChannelsCount() const;

	/**
	 * To stream operator
	 *
	 *	@param std::ostream & os
	 *		The output stream
	 *	@param const Statistician & obj
	 *		The object to stream
	 *
	 *	@return std::ostream &
	 *		The output stream
	 */
	friend std::ostream& operator<<(std::ostream & os, const Statistician & obj);

private:
	/**
	 * The bundle this object acts on
	 *
	 * @var Bundle
	 */
	const structures::Bundle & bundle;

	int clusterCount;
	int inputFibresCount;
	int outputFibresCount;
	int normalFibresCount;
	int inputChannelsCount;
	int outputChannelsCount;

};

}//NAMESPACE

}//NAMESPACE

#endif /* STATISTICIAN_H_ */
