#ifndef gpio_INC
#define gpio_INC

#include "gpio.h"

namespace gpio {

Gpio::Gpio() : 
  m_pin(UNDEFINED), 
  m_mode(0)
{
}

Gpio::~Gpio()
{
 setPin(UNDEFINED);
}

void Gpio::setPin(int pin)
{
 m_pin = pin;
}

void Gpio::setMode(int mode)
{
 m_mode = mode;
} 


}

#endif
