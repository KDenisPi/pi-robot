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
const std::string Gpio::to_string()
{
	return "GPIO_" + std::to_string(m_pin) + " Prov:" + m_prov->to_string();
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
void Gpio::pullUpDnControl(PULL_MODE pumode){
	m_prov->pullUpDownControl(m_pin, pumode);
}
/*
 *
 */
void Gpio::digitalWrite(int value){
	m_prov->dgtWrite(m_pin, value);
}

/*
 *
 */
void Gpio::digitalWritePWM(float dutyCycle, float phaseOffset){
	m_prov->dgtWritePWM(m_pin, dutyCycle, phaseOffset);
}


/*
 *
 */
void Gpio::setPulse(const uint16_t pulselen){
	m_prov->setPulse(m_pin, pulselen);
}


} /*namespace gpio*/
}
#endif
