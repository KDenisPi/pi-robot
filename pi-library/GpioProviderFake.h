/*
 * GpioProviderFake.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
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

	virtual const int dgtRead(const int pin) override;
	virtual void dgtWrite(const int pin, const int value) override;
	virtual void setmode(const int pin, const GPIO_MODE mode) override;
	virtual void pullUpDownControl(const int pin, const PULL_MODE pumode) override;
	virtual void setPulse(const int pin, const uint16_t pulselen) override;

	virtual const std::string to_string() override;
};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERFAKE_H_ */
