/*
 * led.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#include "led.h"

namespace pirobot {
namespace item {

Led::Led(const std::shared_ptr<gpio::Gpio> gpio,
		const LED_STATE init_state,
		const bool init_always) :
	m_gpio(gpio),
	m_state(LED_STATE::OFF),
	m_init_state(init_state),
	m_init_always(init_always)
{
	assert(m_gpio != NULL);
	assert(m_gpio->m_mode ==  gpio::GPIO_MODE::OUT);

	set_name("LED_over_" + m_gpio->toString());
}


Led::Led(const std::shared_ptr<gpio::Gpio> gpio,
		const std::string name,
		const std::string comment,
		const LED_STATE init_state,
		const bool init_always)
			: Item(name, comment),
			  m_gpio(gpio),
			  m_init_state(init_state),
			  m_state(LED_STATE::OFF),
			  m_init_always(init_always)
{
	assert(m_gpio != NULL);
	assert(m_gpio->m_mode ==  gpio::GPIO_MODE::OUT);

	if(name.empty())
		set_name("LED_over_" + m_gpio->toString());
}


Led::~Led() {
	// TODO Auto-generated destructor stub
}

bool Led::initialize(void)
{
	if((m_state != m_init_state) || m_init_always ){
		set_state(m_init_state);
	}
}

/*
 *
 */
const std::string Led::to_string(){
	return name() + (m_state == LED_STATE::OFF ? " OFF" : " ON");
}

void Led::set_state(const LED_STATE state){

	if(m_state != state){
		m_gpio->digitalWrite(state);
		m_state = state;
	}
}


} /* namespace item */
} /* namespace pirobot */
