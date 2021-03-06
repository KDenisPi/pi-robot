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
	GpioProviderFake(const std::string name = "Fake", const int pins=s_pins);
	virtual ~GpioProviderFake();

	virtual const int dgtRead(const int pin) override;
	virtual void dgtWrite(const int pin, const int value) override;
	virtual void setmode(const int pin, const GPIO_MODE mode) override;
	virtual void pullUpDownControl(const int pin, const PULL_MODE pumode) override;
	virtual void setPulse(const int pin, const uint16_t pulselen) override;

	virtual const std::string to_string() override;
    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_FAKE; }

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

	static const int s_pins;
};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERFAKE_H_ */
