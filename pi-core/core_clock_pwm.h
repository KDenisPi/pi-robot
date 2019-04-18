/*
 * core_clock_pwm.h
 *
 * BCM2835 ARM Peripherals. PWM clocks
 *
 *  Created on: March 12, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_CLOCK_PWM_H_
#define PI_CORE_CLOCK_PWM_H_

#include "smallthings.h"
#include "logger.h"

#include "core_common.h"

namespace pi_core {

namespace core_pwm {
    class PwmCore;
}

namespace core_clock_pwm {

#define CLOCK_PWMCTL_PHYS        0x7e1010a0      //Clock Manager Audio Clock Control
#define CLOCK_PWMCTL_OFFSET      0x001010a0      //Clock Manager Audio Clock Control. Registers

#define CLOCK_PWMDIV_PHYS        0x7e1010a4      //Clock Manager Audio Clock Divisors
#define CLOCK_PWMDIV_OFFSET      0x001010a4      //Clock Manager Audio Clock Divisors. Registers

struct pwm_clock_t {
    uint32_t _pwmctl;       //Clock Manager Audio Clock Control register
    uint32_t _pwmdiv;       //Clock Manager Audio Clock Divisors register
} __attribute__((packed, aligned(4))) pwm_clock;

#define CM_CLK_CTL_SRC(val)    (val&0xf)
#define CM_CLK_CTL_SRC_GND                          CM_CLK_CTL_SRC(0)  //RW bit 0-3; Clock source; GND (reset 0)
#define CM_CLK_CTL_SRC_OSC                          CM_CLK_CTL_SRC(1)  //RW bit 0-3; Clock source; Oscillator
#define CM_CLK_CTL_SRC_TSTDBG0                      CM_CLK_CTL_SRC(2)  //RW bit 0-3; Clock source; testdebug0
#define CM_CLK_CTL_SRC_TSTDBG1                      CM_CLK_CTL_SRC(3)  //RW bit 0-3; Clock source; testdebug1
#define CM_CLK_CTL_SRC_PLLA                         CM_CLK_CTL_SRC(4)  //RW bit 0-3; Clock source; PLLA per
#define CM_CLK_CTL_SRC_PLLC                         CM_CLK_CTL_SRC(5)  //RW bit 0-3; Clock source; PLLC per
#define CM_CLK_CTL_SRC_PLLD                         CM_CLK_CTL_SRC(6)  //RW bit 0-3; Clock source; PLLD per
#define CM_CLK_CTL_SRC_HDMIAUX                      CM_CLK_CTL_SRC(7)  //RW bit 0-3; Clock source; HDMI auxiliary
#define CM_CLK_CTL_SRC_GNDS                         CM_CLK_CTL_SRC(8)  //RW bit 0-3; Clock source; 8-15 GND

#define CM_CLK_CTL_ENAB                             (1<<4)  //RW bit 4; Enable clock generator (1-enable;0-reset)
#define CM_CLK_CTL_KILL                             (1<<5)  //RW bit 5; Kill clock generator (1-stop and reset the clock generator;0-no action)
//-- bit 6 unused
#define CM_CLK_CTL_BUSY                             (1<<7)  //RO bit 7; Clock generator is running (1-running;0-reset)
#define CM_CLK_CTL_FLIP                             (1<<8)  //RW bit 8; Invert the clock generator output (1-invert;0-reset)
#define CM_CLK_CTL_MASH(val)    ((val&0x3) << 9)
#define CM_CLK_CTL_MASH_IDIV                        CM_CLK_CTL_MASH(0)  //RW bit 10-9;
#define CM_CLK_CTL_MASH_1STAGE                      CM_CLK_CTL_MASH(1)  //RW bit 10-9;
#define CM_CLK_CTL_MASH_2STAGE                      CM_CLK_CTL_MASH(2)  //RW bit 10-9;
#define CM_CLK_CTL_MASH_3STAGE                      CM_CLK_CTL_MASH(3)  //RW bit 10-9;

#define CM_CLK_CTL_PASSWD                           (0x5a << 24)  //W bit 31-24; Clock manager password '5a'

#define CM_CLK_DIV_PASSWD                           (0x5a << 24)  //W bit 31-24; Clock manager password '5a'
#define CM_CLK_DIV_DIVI(val)   ((val & 0x0FFF) << 12)           //RW bir 23-12; Integer part of divisor
#define CM_CLK_DIV_DIVF(val)   (val & 0x0FFF)                   //RW bir 11-0; Fractional part of divisor

/*
* PWM Clock control class
*/
class PwmClock{
public:
    PwmClock() : _pwm_clock(nullptr) {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        uint32_t ph_address = CoreCommon::get_peripheral_address();
        _pwm_clock = piutils::map_memory<struct pwm_clock_t>(ph_address + clock_pwmctl);

        if(_pwm_clock == nullptr){
            logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Fail to initialize PWM Clock ");
            throw std::runtime_error(std::string("Fail to initialize PWM Clock"));
        }
    }

    virtual ~PwmClock() {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        if(_pwm_clock){
            /*
            * Stop clock
            */
            _stop();

            //Unmap
            _pwm_clock = piutils::unmap_memory<struct pwm_clock_t>(static_cast<struct pwm_clock_t*>((void*)_pwm_clock));
        }
    }

    friend class pi_core::core_pwm::PwmCore;

    const uint32_t clock_pwmctl = CLOCK_PWMCTL_OFFSET;
    const uint32_t clock_pwmctl_phys = CLOCK_PWMCTL_PHYS;

    const uint32_t default_base_freq = 19200000;
    const uint32_t work_freq = 800000;

    /*
    * Initialize connection
    */
   bool Initialize() {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        /*
        * Stop clock before initializing
        */
        _stop();

        return _configure(work_freq, default_base_freq);
    }

protected:

    const bool _is_init() const{
        return (_pwm_clock != nullptr);
    }

    /*
    * List functions for changing control parameters
    */
    void _stop() {
        logger::log(logger::LLOG::INFO, "PwmClock", std::string(__func__));

        if(!_is_init()){
          logger::log(logger::LLOG::INFO, "PwmClock", std::string(__func__) + " Not initialized");
          return;
        }

        /*
        * Set KILL bit and wait
        */
        _pwm_clock->_pwmctl = (CM_CLK_CTL_PASSWD | CM_CLK_CTL_KILL);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //Wait untill BUSY bit will not cleared
        while(((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0)){
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        };

        _pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    /*
    *
    */
    inline const bool _is_busy() const {
        if(_is_init())
            return ((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0);

        return false;
    }

    /*
    * Set initial values for PWM Clock
    */
   bool _configure(const uint32_t freq, const uint32_t base_freq) {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Freq: " + std::to_string(freq) + " Base Freq: " + std::to_string(base_freq));

        uint32_t pdiv = CM_CLK_DIV_PASSWD | CM_CLK_DIV_DIVI(base_freq / (3 * freq));
        _pwm_clock->_pwmdiv = pdiv;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        _pwm_clock->_pwmctl = (CM_CLK_CTL_PASSWD | CM_CLK_CTL_SRC_OSC);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        _pwm_clock->_pwmctl = (CM_CLK_CTL_PASSWD | CM_CLK_CTL_ENAB);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        /*
        * Wait untill BUSY bit will not cleared
        *
        * Just in case check flag during some time
        */
        int i = 0;
        while(((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0) && i < 100){
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            i++;
        };

        if(((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0)){
          logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Could not initialize clock");
          return false;
        }

        return true;
   }


private:
    volatile struct pwm_clock_t* _pwm_clock;
};

} //core_clock_pwm
} //pi_core

#endif
