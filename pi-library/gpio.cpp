#ifndef gpio_INC
#define gpio_INC

#include <string>
#include "gpio.h"

namespace pirobot{
namespace gpio {

/*
 * Constructor
 */
Gpio::Gpio(int pin, GPIO_MODE mode, std::shared_ptr<gpio::GpioProvider> provider) :
  m_pin(pin),
  m_mode(mode),
  m_prov(provider)
{
	m_prov->setmode(pin, mode);
}

/*
 *
 */
Gpio::~Gpio()
{
}

/*
 *
 */
void Gpio::setMode(GPIO_MODE mode)
{
	if(m_mode != mode ){
		m_prov->setmode(m_pin, mode);
		m_mode = mode;
	}
} 

/*
 *
 */
const std::string Gpio::toString()
{
	return "GPIO_" + std::to_string(m_pin);
}

/*
 *
 */
const int Gpio::digitalRead(){
	return m_prov->dgtRead(m_pin);
}

/*
 *
 */
void Gpio::digitalWrite(int value){
	m_prov->dgtWrite(m_pin, value);
}


} /*namespace gpio*/
}
#endif
