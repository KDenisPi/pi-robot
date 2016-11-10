/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#include <unistd.h>
#include <wiringPi.h>

#include "button.h"
#include "logger.h"


namespace pirobot {
namespace item {

const char TAG[] = "button";

/*
 *
 */
Button::Button(const std::shared_ptr<gpio::Gpio> gpio,
               const BUTTON_STATE state,
			   const gpio::PULL_MODE pullmode) :
	m_pullmode(pullmode),
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
        const BUTTON_STATE state,
	    const gpio::PULL_MODE pullmode) :
			m_pullmode(pullmode),
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
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Started.");

}

/*
 *
 */
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
	bool ret = true;

	/*
	 * Set PULL MODE
	 */
	m_gpio->pullUpDnControl(m_pullmode);
	int level = m_gpio->digitalRead();
	set_state((level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED));

	if(is_stopped()){
		m_stopSignal = false;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		int result = pthread_create(&this->m_pthread, &attr, Button::worker, (void*)(this));
		if(result == 0){
			logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Thread created");
		}
		else{
			//TODO: Exception
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Thread failed Res:" + std::to_string(result));
			ret = false;
		}
	}
	return ret;
}

/*
 *
 */
const std::string Button::to_string(){
	return name() + (m_state == BUTTON_STATE::BTN_PUSHED ? " state: PUSHED" : " state: NOT PUSHED");
}

/*
 *
 */
void Button::set_state(const BUTTON_STATE state){
	logger::log(logger::LLOG::DEBUD, TAG, " State changed from: " + std::to_string(m_state) + " to: " + std::to_string(state));
	m_state = state;
}


/*
 *
 */
void* Button::worker(void* p){
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker started.");

	Button* owner = static_cast<Button*>(p);
	while(!owner->is_stopSignal()){
		int level = owner->m_gpio->digitalRead();
		const BUTTON_STATE state = (level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED);
		if(state != owner->state()){
			owner->set_state(state);
		}

		delay(10);
	}

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}

} /* namespace item */
} /* namespace pirobot */
