/*
 * led.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#include "logger.h"
#include "led.h"

namespace pirobot {
namespace item {

const char TAG[] = "led";

Led::Led(const std::shared_ptr<gpio::Gpio> gpio,
        const std::string name,
        const std::string comment,
        const LED_STATE init_state,
        const bool init_always)
            : Item(gpio, name, comment, ItemTypes::LED),
              m_init_state(init_state),
              m_state(LED_STATE::OFF),
              m_init_always(init_always)
{
    assert(get_gpio() != NULL);
    assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

    if(name.empty())
        set_name("LED_over_" + get_gpio()->to_string());
}

Led::~Led() {
    // TODO Auto-generated destructor stub
}

bool Led::initialize(void)
{
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " State: " + std::to_string(m_state) + " Init:" + std::to_string(m_init_state));

  get_gpio()->pullUpDnControl(gpio::PULL_MODE::PULL_OFF);
  if((m_state != m_init_state) || m_init_always ){
     set_state(m_init_state);
  }
  return true;
}

/*
 *
 */
const std::string Led::to_string(){
    return name() + (m_state == LED_STATE::OFF ? " OFF" : " ON");
}

/*
 *
 */
const std::string Led::printConfig(){
    return name() + " GPIO: " + get_gpio()->to_string();
}

void Led::set_state(const LED_STATE state){

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " State from: " + std::to_string(m_state) + " to:" + std::to_string(state));

  get_gpio()->digitalWrite(state);
  m_state = state;
}


} /* namespace item */
} /* namespace pirobot */
