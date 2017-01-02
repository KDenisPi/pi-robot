/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
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
	void* ret;
	int res = 0;

	set_stop_signal(true);
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->get_thread()));

	if( !is_stopped() ){
		res = pthread_join(this->get_thread(), &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
bool Button::initialize(void)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started...");
	bool ret = true;

	/*
	 * Set PULL MODE
	 */
	get_gpio()->pullUpDnControl(m_pullmode);
	int level = get_gpio()->digitalRead();
	set_state((level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED));

	if(is_stopped()){
		set_stop_signal(false);

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_t pthread;
		int result = pthread_create(&pthread, &attr, Button::worker, (void*)(this));
		if(result == 0){
			set_thread(pthread);
			logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Thread created");
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
	while(!owner->is_stopSignal()){
		int level = owner->get_gpio()->digitalRead();
		const BUTTON_STATE state = (level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED);
	        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ** State :" + std::to_string(state));
		if(state != owner->state()){
			owner->set_state(state);
   	                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ** State changed!!!! " + owner->name());

	   	        std::string name = owner->name();
			if(owner->notify)
	   		        owner->notify(owner->type(), name, (void*)(&state));
		}

		delay(100);
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}

} /* namespace item */
} /* namespace pirobot */
