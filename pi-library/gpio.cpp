#ifndef gpio_INC
#define gpio_INC

#include <string>

#include "gpio.h"

namespace pirobot{
namespace gpio {

const char TAG[] = "GPIO";

/*
 * Constructor
 */
Gpio::Gpio(const int pin, const GPIO_MODE mode, const std::shared_ptr<gpio::GpioProvider> provider, const PULL_MODE pull_mode, const GPIO_EDGE_LEVEL edge_level) :
  m_pin(pin),
  m_mode(mode),
  m_prov(provider),
  _pull_mode(pull_mode),
  _edge_level(edge_level)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" pin: ") + std::to_string(pin) + " mode: " + std::to_string((int)mode) +
			" pull mode: " + std::to_string((int)pull_mode));

	m_prov->setmode(pin, mode);
	if(PULL_MODE::PULL_OFF != _pull_mode){
		pullUpDnControl(_pull_mode);
	}
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
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " set mode: " + std::to_string((int)mode));
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
void Gpio::pullUpDnControl(PULL_MODE pull_mode){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " set pull mode: " + std::to_string((int)pull_mode));
	m_prov->pullUpDownControl(m_pin, pull_mode);
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
