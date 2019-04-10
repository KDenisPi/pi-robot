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

#include "core_common.h"
#include "smallthings.h"
#include "logger.h"

namespace pi_core {
namespace core_clock_pwm {

#define CLOCK_PWMCTL_PHYS        0x7e1010a0      //Clock Manager Audio Clock Control
#define CLOCK_PWMCTL_OFFSET      0x001010a0      //Clock Manager Audio Clock Control. Registers

#define CLOCK_PWMDIV_PHYS        0x7e1010a4      //Clock Manager Audio Clock Divisors
#define CLOCK_PWMDIV_OFFSET      0x001010a4      //Clock Manager Audio Clock Divisors. Registers

struct pwm_clock_t {
    uint32_t _pwmctl;       //Clock Manager Audio Clock Control register
    uint32_t _pwmdiv;       //Clock Manager Audio Clock Divisors register
} __attribute__((packed, aligned(4))) pwm_clock;

#define CM_CLK_CTL_SRC_GND                          0x00000000  //RW bit 0-3; Clock source; GND (reset 0)
#define CM_CLK_CTL_SRC_OSC                          0x00000001  //RW bit 0-3; Clock source; Oscillator
#define CM_CLK_CTL_SRC_TSTDBG0                      0x00000002  //RW bit 0-3; Clock source; testdebug0
#define CM_CLK_CTL_SRC_TSTDBG1                      0x00000003  //RW bit 0-3; Clock source; testdebug1
#define CM_CLK_CTL_SRC_PLLA                         0x00000004  //RW bit 0-3; Clock source; PLLA per
#define CM_CLK_CTL_SRC_PLLC                         0x00000005  //RW bit 0-3; Clock source; PLLC per
#define CM_CLK_CTL_SRC_PLLD                         0x00000006  //RW bit 0-3; Clock source; PLLD per
#define CM_CLK_CTL_SRC_HDMIAUX                      0x00000007  //RW bit 0-3; Clock source; HDMI auxiliary
#define CM_CLK_CTL_SRC_GNDS                         0x00000008  //RW bit 0-3; Clock source; 8-15 GND

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
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " HW Adderess: " + std::to_string(ph_address));

        _pwm_clock = piutils::map_memory<struct pwm_clock_t>(ph_address + clock_pwmctl);
        if(_pwm_clock == nullptr){
            logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Fail to initialize PWM Clock ");
        }
    }

    virtual ~PwmClock() {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        if(_pwm_clock)
            _pwm_clock = piutils::unmap_memory<struct pwm_clock_t>(static_cast<struct pwm_clock_t*>((void*)_pwm_clock));
    }

    const uint32_t clock_pwmctl = CLOCK_PWMCTL_OFFSET;
    const uint32_t clock_pwmctl_phys = CLOCK_PWMCTL_PHYS;

    const uint32_t default_base_freq = 19200000;
    const uint32_t work_freq = 800000;

    /*
    * Initialize connection
    */
   bool Initialize() {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        return _configure(work_freq, default_base_freq);
    }

    const bool _is_init() const{
        return (_pwm_clock != nullptr);
    }


    /*
    * List functions for changing control parameters
    */
    void _stop() {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__));

        if(!_is_init()){
          logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Not initialized");
          return;
        }

        _pwm_clock->_pwmctl = (CM_CLK_CTL_KILL | CM_CLK_CTL_PASSWD);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //TODO: delay too?
        if(_is_busy()){
          logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Could not stop clock");
        }

        _pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //while(_is_busy());
    }

    inline const bool _is_busy() const {
        return ((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0);
    }

#define IS_BUSY(val) (val & CM_CLK_CTL_BUSY)

    /*
    * Set initial values for PWM Clock
    */
   bool _configure(const uint32_t freq, const uint32_t base_freq) {
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Freq: " + std::to_string(freq) + " Base Freq: " + std::to_string(base_freq));
        volatile struct pwm_clock_t* pwm_clock = _pwm_clock; 

        uint32_t pwmctl = 0;
        uint32_t pdiv = CM_CLK_DIV_PASSWD | CM_CLK_DIV_DIVI(base_freq / (3 * freq));
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " DIVI: " + std::to_string(pdiv));

        pwm_clock->_pwmdiv = CM_CLK_DIV_PASSWD | CM_CLK_DIV_DIVI(base_freq / (3 * freq));

        pwmctl = pwm_clock->_pwmctl;
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " 1 : " + std::to_string(pwmctl));

        pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD | CM_CLK_CTL_SRC_OSC;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        if(IS_BUSY(pwm_clock->_pwmctl)){
          logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Busy 2");
        }

        pwmctl = pwm_clock->_pwmctl;
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " 2 : " + std::to_string(pwmctl));

        pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD | CM_CLK_CTL_SRC_OSC | CM_CLK_CTL_ENAB;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        if(IS_BUSY(pwm_clock->_pwmctl)){
          logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Busy 3");
        }

        pwmctl = pwm_clock->_pwmctl;
        logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " 3 : " + std::to_string(pwmctl));

        int i = 0;
        while((pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) && i < 10){
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          logger::log(logger::LLOG::DEBUG, "PwmClock", std::string(__func__) + " Busy Wait...");
          i++;
        }

        if(IS_BUSY(pwm_clock->_pwmctl)){
          logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Could not initialize clock");
          return false;
        }

        return true;
   }


private:
    volatile struct pwm_clock_t* _pwm_clock;
};

}
}

#endif
