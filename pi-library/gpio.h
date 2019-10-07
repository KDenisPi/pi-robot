/*
 * Gpio.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIO_H_
#define PI_LIBRARY_GPIO_H_

#include <memory>

#include "GpioProvider.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

class Gpio
{
  public:

    Gpio(const int pin, const GPIO_MODE mode, const std::shared_ptr<gpio::GpioProvider> provider, 
        const PULL_MODE pull_mode = PULL_MODE::PULL_OFF, 
        const GPIO_EDGE_LEVEL edge_level = GPIO_EDGE_LEVEL::EDGE_LEVEL_OFF);

    virtual ~Gpio();

    const int getPin() const {return m_pin;}
    const GPIO_MODE getMode() const {return m_mode;}
    const GPIO_EDGE_LEVEL get_edge_level() const {return _edge_level;}

   /*
    * Probably not all providers allow change mode (?)
    */
    void setMode(GPIO_MODE mode);

    const int digitalRead();
    void digitalWrite(int value);
    void digitalWritePWM(float dutyCycle, float phaseOffset=0.0f);
    void pullUpDnControl(PULL_MODE pumode);
    void setPulse(const uint16_t pulselen);

    const std::string to_string();

    const GPIO_PROVIDER_TYPE get_provider_type() const { return m_prov->get_type(); }
    const std::shared_ptr<gpio::GpioProvider> get_provider() {return m_prov;}

  /*
  * Change GPIO state
  */
    inline void High() { set_level(gpio::SGN_LEVEL::SGN_HIGH);}
    inline void Low() { set_level(gpio::SGN_LEVEL::SGN_LOW);}

    const bool is_High() {
      auto level = get_level();
      return (level == gpio::SGN_LEVEL::SGN_HIGH);
    }
  /*
  * Get GPIO level
  */
    const gpio::SGN_LEVEL get_level() {
      return (0 == digitalRead() ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
    }

  /*
  * Set GPIO level
  */
    inline void set_level(const gpio::SGN_LEVEL level){
     digitalWrite(level);
    }

  //set edge and level detection for pin (if supported)
	bool set_egde_level(GPIO_EDGE_LEVEL edgs_level){
  	logger::log(logger::LLOG::DEBUG, "GPIO", std::string(__func__) + " set edge/level: " + std::to_string((int)edgs_level));

    if((m_mode != GPIO_MODE::IN) && (m_mode != GPIO_MODE::IN)){
      return false; //we support this functionality for IN/OUT modes only
    }

    bool ret = m_prov->set_egde_level(m_pin, edgs_level);
    if(ret){
      _edge_level = edgs_level;
    }

  	logger::log(logger::LLOG::DEBUG, "GPIO", std::string(__func__) + " edge/level: " + std::to_string((int)_edge_level) + " result: " +  std::to_string(ret));
  }


  private:
    int m_pin;
    GPIO_MODE m_mode; //INPUT/OUTPUT
    PULL_MODE _pull_mode; //OFF/DOWN/UP
    GPIO_EDGE_LEVEL _edge_level; //control change GPIO level using interrupt processing (if provided supported it)
    
    std::shared_ptr<gpio::GpioProvider> m_prov; //provider
};

}
}

#endif /* PI_LIBRARY_GPIO_H_ */
