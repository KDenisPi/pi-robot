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

namespace pirobot {
namespace gpio {

class Gpio
{
  public:

    Gpio(int pin, GPIO_MODE mode, const std::shared_ptr<gpio::GpioProvider> provider);
    virtual ~Gpio();

   const int getPin() const {return m_pin;};
   const GPIO_MODE getMode() const {return m_mode;};

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

  /*
  * Get GPIO level
  */
   const gpio::SGN_LEVEL get_level() {
      return (0 == digitalRead() ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
   }

  /*
  * Set GPIO level
  */
   void set_level(const gpio::SGN_LEVEL level){
     digitalWrite(level);
   }

  private:
   int m_pin;
   GPIO_MODE m_mode; //INPUT/OUTPUT

   std::shared_ptr<gpio::GpioProvider> m_prov;
};

}
}

#endif /* PI_LIBRARY_GPIO_H_ */
