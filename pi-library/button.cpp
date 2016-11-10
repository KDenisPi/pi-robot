/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#include <unistd.h>

#include "button.h"
#include "logger.h"


namespace pirobot {
namespace item {

const char TAG[] = "button";

/*
 *
 */
Button::Button(const std::shared_ptr<gpio::Gpio> gpio,
               const BUTTON_STATE state) :
	m_gpio(gpio),
    m_state(state),
	m_pthread(0),
	m_stopSignal(false)
{
	assert(m_gpio != NULL);
	assert(m_gpio->getMode() ==  gpio::GPIO_MODE::IN);

	set_name("BTN_over_" + m_gpio->toString());
}

/*
 *
 */
Button::Button(const std::shared_ptr<gpio::Gpio> gpio,
		const std::string name,
		const std::string comment,
        const BUTTON_STATE state) :
        	Item(name, comment),
			m_gpio(gpio),
			m_state(state),
			m_pthread(0),
			m_stopSignal(false)
{
	assert(m_gpio != NULL);
	assert(m_gpio->getMode() ==  gpio::GPIO_MODE::IN);

	if(name.empty())
		set_name("BTN_over_" + m_gpio->toString());
}

/*
 *
 */
Button::~Button() {
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Started...");

}

void Button::stop(){
	void* ret;
	int res = 0;

	m_stopSignal = true;
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->m_pthread));

	if( !is_stopped() ){
		res = pthread_join(this->m_pthread, &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
	}

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
bool Button::initialize(void)
{
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Started...");

	if(is_stopped()){
		m_stopSignal = false;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		int result = pthread_create(&this->m_pthread, &attr, Button::worker, (void*)(this));
		if(result == 0){
			logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Thread created");
		}
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
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker started...");

	std::shared_ptr<Button> owner(static_cast<Button*>(p));
	while(!owner->is_stopSignal()){
		sleep(1);
		logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Sleep loop....");
	}

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker finished...");
	return (void*) 0L;
}

} /* namespace item */
} /* namespace pirobot */
