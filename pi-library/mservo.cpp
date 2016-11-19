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

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio) :
	Item(gpio)
{
}

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,	const std::string comment) :
	Item(gpio, name, comment)
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
 * Set pulse in milliseconds
 */
void ServoMotor::setPulse(const uint16_t pulselen){
	get_gpio()->setPulse(pulselen);
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
