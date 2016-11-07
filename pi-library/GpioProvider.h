/*
 * GpioProvider.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDER_H_
#define PI_LIBRARY_GPIOPROVIDER_H_

#include <string>

namespace gpio {

class GpioProvider {
public:
	GpioProvider() {}
	virtual ~GpioProvider() {}

	virtual const std::string toString() = 0;

	virtual const int dgtRead(int pin) = 0;
	virtual void dgtWrite(int pin, int value) = 0;
	virtual void pinmode(int pin, int mode) = 0;

};

}
#endif /* PI_LIBRARY_GPIOPROVIDER_H_ */
