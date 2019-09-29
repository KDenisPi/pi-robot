/*
 * GpioProviderSimple.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERSIMPLE_H_
#define PI_LIBRARY_GPIOPROVIDERSIMPLE_H_

#include <cstdlib>

#include "GpioProvider.h"
#include "smallthings.h"
#include "timers.h"

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

    virtual const GPIO_MODE getmode(const int pin) override;

    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_SIMPLE; }
    virtual const std::string to_string() override;

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

	/*
	* Some GPIO providers supoprt level detection through interrupt
	* It can be useful for detection multiple states changing (usually IN; for example buttons) instead of polling
	*/
    const size_t s_buff_size = 32;

	virtual bool export_gpio(const int pin) override {
        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" pin: ") + std::to_string(pin));

        bool result = false;
        auto pin_dir = get_gpio_path(pin);
        
        if(!piutils::chkfile(pin_dir)){
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " No folder for pin: " + pin_dir);
            std::string command = std::string("echo ") + std::to_string(pin) + std::string(" > /sys/class/gpio/export");

            int res = std::system(command.c_str());
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Executed command: " + command + " result: " + std::to_string(res));

            piutils::timers::Timers::delay(500);

            //check result
            if(!piutils::chkfile(pin_dir)){
                logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + std::string(" Could not create folder: ") + pin_dir);
                return result;
            }
        }


        //GPIO exported. Add it to pool

        return result;
    }

	virtual bool unexport_gpio(const int pin) override {
        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" pin: ") + std::to_string(pin));

        //
        //Stop check procedure
        //

        auto pin_dir = get_gpio_path(pin);
        if(piutils::chkfile(pin_dir)){
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Folder exist: " + pin_dir);
            std::string command = std::string("echo ") + std::to_string(pin) + std::string(" > /sys/class/gpio/unexport");

            int res = std::system(command.c_str());
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Executed command: " + command + " result: " + std::to_string(res));

            piutils::timers::Timers::delay(500);

            if(piutils::chkfile(pin_dir)){
                logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + std::string(" Could not delete folder: ") + pin_dir);
                return false;
            }
        }

        return true;
    }
	
    virtual bool is_support_group() override {
        return true;
    }


    /*
    * Print mode for each GPIO PIN
    */
    const std::string print_mode() {
      std::string result;

      for(int i = 0; i < pins(); i++){
           result +=  "Pin: " + std::to_string(i) + " [" + std::to_string(phys_pin(i)) + "] Mode: " + std::to_string(getmode(i)) + "\n";
      }
      return result;
    }

private:
    volatile gpio_ctrl* _gctrl; //pointer to GPIO registers map
    std::mutex _mx_gpio;

    const int phys_pin(const int pin) const {
        return pins_map[getRealPin(pin)];
    }

    //Prepare directory name for PIN
    const std::string get_gpio_path(const int pin){
        char buff[s_buff_size];
        snprintf(buff, s_buff_size, "/sys/class/gpio/gpio%d/value", pin);
        std::string pin_dir = buff;

        return pin_dir;
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
