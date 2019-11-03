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
  _pull_mode(PULL_MODE::PULL_OFF),
  _edge_level(GPIO_EDGE_LEVEL::EDGE_NONE)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" pin: ") + std::to_string(pin) + " mode: " + std::to_string((int)mode) +
			" pull mode: " + std::to_string((int)pull_mode));

	//set low level name
	_name = get_gpio_name(m_prov->get_name(), m_pin);

	//set GPIO mode
	m_prov->setmode(pin, mode);

	//set GPIO pull mode	
	if(PULL_MODE::PULL_OFF != pull_mode){
		pullUpDnControl(pull_mode);
	}

	//set GPIO edge level
	if(GPIO_EDGE_LEVEL::EDGE_NONE != edge_level){
		set_egde_level(edge_level);
	}
}

/*
 *
 */
Gpio::~Gpio()
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " edge: " + std::to_string((int) get_edge_level()));

	if(GPIO_EDGE_LEVEL::EDGE_NONE != get_edge_level()){
		set_egde_level(GPIO_EDGE_LEVEL::EDGE_NONE);
	}
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
const int Gpio::digitalRead(){
	return m_prov->dgtRead(m_pin);
}

/*
 *
 */
void Gpio::pullUpDnControl(PULL_MODE pull_mode){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " set pull mode: " + std::to_string((int)pull_mode));
	if(_pull_mode != pull_mode){
		m_prov->pullUpDownControl(m_pin, pull_mode);
		_pull_mode = pull_mode;
	}
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
