/*
 * GpioProviderSimple.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDERSIMPLE_H_
#define PI_LIBRARY_GPIOPROVIDERSIMPLE_H_

#include "GpioProvider.h"

namespace pirobot {
namespace gpio {

class GpioProviderSimple : public GpioProvider
{
public:
	GpioProviderSimple();
	virtual ~GpioProviderSimple();

	virtual const int dgtRead(const int pin);
	virtual void dgtWrite(const int pin, const int value);
	virtual void setmode(const int pin, const GPIO_MODE mode);
	virtual void pullUpDownControl(const int pin, const PULL_MODE pumode);

	virtual const std::string to_string();
};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERSIMPLE_H_ */
