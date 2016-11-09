/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#include <unistd.h>
#include "button.h"

namespace pirobot {
namespace item {


Button::Button(const std::shared_ptr<gpio::Gpio> gpio,
               const BUTTON_STATE state) :
	m_gpio(gpio),
    m_state(state),
	m_pthread(0),
	m_stop(false)
{
	assert(m_gpio != NULL);
	assert(m_gpio->getMode() ==  gpio::GPIO_MODE::IN);

	set_name("BTN_over_" + m_gpio->toString());
}


Button::Button(const std::shared_ptr<gpio::Gpio> gpio,
		const std::string name,
		const std::string comment,
        const BUTTON_STATE state) :
        	Item(name, comment),
			m_gpio(gpio),
			m_state(state),
			m_pthread(0),
			m_stop(false)
{
	assert(m_gpio != NULL);
	assert(m_gpio->getMode() ==  gpio::GPIO_MODE::IN);

	if(name.empty())
		set_name("BTN_over_" + m_gpio->toString());
}


Button::~Button() {
	void* ret;

	stop();
	int res = pthread_join(this->m_pthread, &ret);
}

bool Button::initialize(void)
{
	if(m_pthread == 0){
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		int result = pthread_create(&this->m_pthread, &attr, Button::worker, (void*)(this));
	}
	return true;
}

/*
 *
 */
const std::string Button::to_string(){
	return name() + (m_state == BUTTON_STATE::DISCONNECTED ? " OFF" : " ON");
}

/*
 *
 */
void* Button::worker(void* p){
	std::shared_ptr<Button> owner(static_cast<Button*>(p));

	while(!owner->is_stopped()){
		sleep(1);
	}

}

} /* namespace item */
} /* namespace pirobot */
