/*
 * GpioProvider.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDER_H_
#define PI_LIBRARY_GPIOPROVIDER_H_

#include <string>

namespace pirobot{
namespace gpio {

enum GPIO_MODE {
	IN = 0,
	OUT = 1
};

class GpioProvider {
public:
	GpioProvider() {}
	virtual ~GpioProvider() {}

	virtual const std::string toString() = 0;
	virtual const int dgtRead(int pin) = 0;
	virtual void dgtWrite(int pin, int value) = 0;
	virtual void setmode(int pin, GPIO_MODE mode) = 0;

};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDER_H_ */
