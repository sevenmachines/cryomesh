/**
 * Maths.h
 *
 *  Created on: 27 Jan 2011
 *      Author: SevenMachines<SevenMachines@yahoo.co.uk>
 */

#ifndef MATHS_H_
#define MATHS_H_

namespace cryomesh {

namespace common {

/**
 *  Maths contains some useful maths functions and algorithmns
 */
class Maths {
public:
	Maths();
	virtual ~Maths();

	/**
	 * Compare doubles using delta
	 *
	 * @param const double lhs
	 * 		lhs comparator
	 * @param const double rhs
	 * 		rhs comparator
	 * @param const double delta
	 * 		comparison delta
	 *
	 * @return bool
	 * 		True if lhs and rhs compare within delta margin, false otherwise
	 */
	static bool compareDoubles(const double lhs, const double rhs, const double delta = 0.00001) {
		//std::cout<<"Maths::compareDoubles: "<<lhs<<"==" << rhs <<" ~ "<<delta<<std::endl;
		if ((lhs < rhs - delta) || (lhs < rhs + delta)) {
			return false;
		}
		return true;
	}
	static int getRandomInteger(int low, int high) {
		double temp = 0;
		// swap low & high around if the user makes no sense
		if (low > high) {
			temp = low;
			low = high;
			high = temp;
		}
		int val = rand() % (high - low + 1) + low;
		return val;
	}
	static double getRandomDouble(double low, double high) {
		double temp = 0;
		// swap low & high around if the user makes no sense
		if (low > high) {
			temp = low;
			low = high;
			high = temp;
		}

		double val = (rand() / (static_cast<double> (RAND_MAX) + 1.0)) * (high - low) + low;
		return val;
	}
	static bool getRandomBool(double fraction = 0.5) {
		return getRandomDouble(0, 1) >= 0.5 ? true : false;
	}
};

}

}

#endif /* MATHS_H_ */
