/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#include <cassert>
#include <unistd.h>
#include <wiringPi.h>

#include "button.h"
#include "logger.h"


namespace pirobot {
namespace item {

const char TAG[] = "button";

const char* Item::ItemNames[] = {"UNK", "LED", "BTN", "SERV", "TILT", "STEP"};

/*
 *
 */
Button::Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
               	const BUTTON_STATE state,
		const gpio::PULL_MODE pullmode,
		const int itype) :
	Item(gpio, itype),
	m_pullmode(pullmode),
    m_state(state)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::IN);

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");

	set_name(type_name() + "_over_" + get_gpio()->to_string());
}

/*
 *
 */
Button::Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,
		const std::string comment,
        const BUTTON_STATE state,
	    const gpio::PULL_MODE pullmode,
		const int itype) :
           	Item(gpio, name, comment, itype),
			m_pullmode(pullmode),
			m_state(state)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::IN);

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");

	if(name.empty())
		set_name(type_name()  + "_over_" + get_gpio()->to_string());
}

/*
 *
 */
Button::~Button() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");

}

/*
 *
 */
void Button::stop(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");
	piutils::Threaded::stop();
}

/*
 *
 */
bool Button::initialize(void)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started...");

	/*
	 * Set PULL MODE
	 */
	get_gpio()->pullUpDnControl(m_pullmode);
	int level = get_gpio()->digitalRead();

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Current state:" + std::to_string(level));
	set_state((level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED));
	return piutils::Threaded::start<Button>(this);
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
const std::string Button::printConfig(){
        return name() + " GPIO: " + get_gpio()->to_string();
}

/*
 *
 */
void Button::set_state(const BUTTON_STATE state){
	logger::log(logger::LLOG::DEBUG, TAG, " State changed from: " + std::to_string(m_state) + " to: " + std::to_string(state));
	m_state = state;
}


/*
 *
 */
void* Button::worker(void* p){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started.");

	Button* owner = static_cast<Button*>(p);
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ** Initial State :" + std::to_string(owner->state()));
	while(!owner->is_stop_signal()){
		int level = owner->get_gpio()->digitalRead();
		const BUTTON_STATE state = (level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED);
	        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ** State :" + std::to_string(state));
		if(state != owner->state()){
			owner->set_state(state);
   	                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ** State changed!!!! " + owner->name() +
				" New state:" + std::to_string(state));

	   	        std::string name = owner->name();
			if(owner->notify)
	   		        owner->notify(owner->type(), name, (void*)(&state));
		}

		delay(owner->get_loopDelay());
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}

} /* namespace item */
} /* namespace pirobot */
