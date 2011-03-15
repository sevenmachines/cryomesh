/*
 * Degree.h
 *
 *  Created on: 4 May 2010
 *      Author: niall
 */

#ifndef DEGREE_H_
#define DEGREE_H_

namespace cryomesh {

namespace spacial {

class Degree {
public:
	Degree(double d);
	virtual ~Degree();
	double getValue()const;
	double toRadians()const;
private:
	double value;
};

}

}

#endif /* DEGREE_H_ */
