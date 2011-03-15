/*
 * Radian.h
 *
 *  Created on: 4 May 2010
 *      Author: niall
 */

#ifndef RADIAN_H_
#define RADIAN_H_

namespace cryomesh {

namespace spacial {

class Radian {
public:
	Radian(double d);
	virtual ~Radian();
	double getValue()const;
	double toDegrees()const;

	bool operator==(const Radian & obj) const;
	Radian & operator=(const Radian & obj);
	Radian operator+(const Radian & obj)const;
	Radian operator-(const Radian & obj)const;

	static const double PI ;
private:
	double value;

};

}

}
#endif /* RADIAN_H_ */
