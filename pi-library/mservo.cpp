/*
 * ServoMotor.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: denis
 */


#include "mservo.h"

const char TAG[] = "Servo";

namespace pirobot {
namespace item {

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
	const uint16_t offset,
	const uint16_t max) :
	Item(gpio), m_max(max), m_offset(offset)
{
}

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,	
		const std::string comment,
		const uint16_t offset,
		const uint16_t max) :
	Item(gpio, name, comment), m_max(max), m_offset(offset)
{

}

/*
 *
 */
ServoMotor::~ServoMotor(){

}

bool ServoMotor::initialize(){
  return true;
}

void ServoMotor::stop(){

} 

/*
* Get current position of servi drive
*/
const uint16_t ServoMotor::get_curent(){
  return get_gpio()->digitalRead();
}

/*
*
*/
const uint16_t ServoMotor::set_position(const uint16_t pos){
  uint16_t cpos = get_gpio()->digitalRead();
  if(pos < 0 || pos > m_max || cpos == pos){
   return cpos;
  }

  if(pos > cpos){
    for(uint16_t npos = cpos; npos <= pos; npos++)
      get_gpio()->digitalWrite(npos);
  }
  else{
    for(uint16_t npos = cpos; npos > pos; npos--)
      get_gpio()->digitalWrite(npos);
  }

  return get_gpio()->digitalRead();
}

/*
 * Set pulse in milliseconds
 */
void ServoMotor::setPulse(const uint16_t pulselen){
	//get_gpio()->setPulse(pulselen);
}

/*
 *
 */
const std::string ServoMotor::to_string(){
	return name();
}

/*
 *
 */
const std::string ServoMotor::printConfig(){
	return name() + " GPIO: " + get_gpio()->to_string();
}

} /* namespace item */
} /* namespace pirobot */
