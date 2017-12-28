/*
 * GpioProviderSimple.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERSIMPLE_H_
#define PI_LIBRARY_GPIOPROVIDERSIMPLE_H_

#include "GpioProvider.h"

namespace pirobot {
namespace gpio {

class GpioProviderSimple : public GpioProvider
{
public:
    GpioProviderSimple(const std::string name = "SIMPLE", const int pins = 12);
    virtual ~GpioProviderSimple();

    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void setmode(const int pin, const GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const PULL_MODE pumode) override;
    virtual void setPulse(const int pin, const uint16_t pulselen) override;

    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_SIMPLE; }
    virtual const std::string to_string() override;

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }
};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERSIMPLE_H_ */
