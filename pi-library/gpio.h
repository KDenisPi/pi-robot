/*
 * Gpio.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
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

   virtual const int digitalRead();
   virtual void digitalWrite(int value);

   const std::string toString();

  private:
   int m_pin;
   GPIO_MODE m_mode; //INPUT/OUTPUT

   std::shared_ptr<gpio::GpioProvider> m_prov;
};

}
}

#endif /* PI_LIBRARY_GPIO_H_ */
