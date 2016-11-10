/*
 * GpioProviderFake.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDERFAKE_H_
#define PI_LIBRARY_GPIOPROVIDERFAKE_H_

#include "GpioProvider.h"

namespace pirobot{
namespace gpio {

class GpioProviderFake : public GpioProvider
{
public:
	GpioProviderFake();
	virtual ~GpioProviderFake();

	virtual const int dgtRead(int pin);
	virtual void dgtWrite(int pin, int value);
	virtual void setmode(int pin, GPIO_MODE mode);
	virtual const std::string toString();

};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERFAKE_H_ */
