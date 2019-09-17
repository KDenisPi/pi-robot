/*
 * GpioProviderSimple.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERSIMPLE_H_
#define PI_LIBRARY_GPIOPROVIDERSIMPLE_H_

#include "GpioProvider.h"
#include "smallthings.h"

namespace pirobot {
namespace gpio {

/*
* BCM2835 ARM peripherials (section 6.1 Register View)
*/
using gpio_ctrl = struct __attribute__((packed, aligned(4))) gpio_ctrl {
    uint32_t _GPFSEL[6];  //GPIO Function Select 0,1,2,3,4,5
    uint32_t _reserved1;

    uint32_t _GPSET[2];   //GPIO Pin Output Set 0,1 (W)
    uint32_t _reserved2;

    uint32_t _GPCLR[2];   //GPIO Pin Output Clear 0,1 (W)
    uint32_t _reserved3;

    uint32_t _GPLEV[2];   //GPIO Pin Level 0,1 (R)
    uint32_t _reserved4;

    uint32_t _GPEDS[2];   //GPIO Pin Event Detect Status 0,1
    uint32_t _reserved5;

    uint32_t _GPREN[2];   //GPIO Pin Rising Edge Detect Enable 0,1
    uint32_t _reserved6;

    uint32_t _GPFEN[2];   //GPIO Pin Falling Edge Detect Enable 0,1
    uint32_t _reserved7;

    uint32_t _GPHEN[2];   //GPIO Pin High Detect Enable 0,1
    uint32_t _reserved8;

    uint32_t _GPLEN[2];   //GPIO Pin Low Detect Enable 0,1
    uint32_t _reserved9;

    uint32_t _GPAREN[2];  //GPIO Pin Async. Rising Edge Detect 0,1
    uint32_t _reserved10;

    uint32_t _GPAFEN[2];  //GPIO Pin Async. Falling Edge Detect 0,1
    uint32_t _reserved11;

    uint32_t _GPPUD;    //GPIO Pin Pull-up/down Enable

    uint32_t _GPPUDCLK[2];// GPIO Pin Pull-up/down Enable Clock 0,1
    uint32_t _reserved12;
};

class GpioProviderSimple : public GpioProvider
{
public:
    GpioProviderSimple(const std::string name = "SIMPLE", const int pins = 26) noexcept(false);
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

private:
    volatile gpio_ctrl* _gctrl; //pointer to GPIO registers map
    std::mutex _mx_gpio;

    const int phys_pin(const int pin) const {
        return pins_map[getRealPin(pin)];
    }

    /*
        GPIO
        General     0-6   GPIO [17, 18, 27, 22, 23, 24, 25]
        Additional  7-13  GPIO [ 5, 6,  19, 16, 26, 20, 21]
        PWM         14,15 GPIO [12, 13]
        I2C         16,17 GPIO [ 2, 3]
        SPI         18-22 GPIO [ 7, 8, 9, 10, 11] CE1_N, CE0_N, SO, SI, SCLK
        GCLK        23         [4]
        UART TX/RX  24,25      [14, 15]
    */
    const int pins_map[26] = {
        17, 18, 27, 22, 23, 24, 25, 5, 6, 19, 16, 26, 20, 21,
        12, 13,
        2,  3,
        7,  8,  9, 10, 11,
        4,
        14, 15
    };

    using pin_mode = std::tuple<int, GPIO_MODE>;

    const pin_mode pins_pwm[4] = {
        std::make_tuple(12, GPIO_MODE::ALT0),
        std::make_tuple(18, GPIO_MODE::ALT5),
        std::make_tuple(13, GPIO_MODE::ALT0),
        std::make_tuple(18, GPIO_MODE::ALT5)
    };

    GPIO_MODE get_pwm_mode(const int pin) const {
        for(int i = 0; i < 4; i++){
            if(std::get<0>(pins_pwm[i]) == pin)
                return std::get<1>(pins_pwm[i]);
        }

        return GPIO_MODE::PWM_OUT;
    }

};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERSIMPLE_H_ */
