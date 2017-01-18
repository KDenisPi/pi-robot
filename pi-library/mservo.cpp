/*
 * ServoMotor.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: denis
 */


#include "mservo.h"
#include "logger.h"

const char TAG[] = "Servo";

namespace pirobot {
namespace item {

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
	const float plusMinusRange) :
	Item(gpio, ItemTypes::SERVO),
	m_plusMinusRange(plusMinusRange),
	m_minDutyCycle(0.0f),
	m_maxDutyCycle(0.0f),
	m_zeroDutyCycle(0.0f),
	m_angleStepSize(0.0f)
{
   this->calibrate(3.1f, 13.80f); //11.75f); // Typical values
}

ServoMotor::ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,	
		const std::string comment,
		const float plusMinusRange) :
	Item(gpio, name, comment, ItemTypes::SERVO),
	m_plusMinusRange(plusMinusRange),
	m_minDutyCycle(0.0f),
	m_maxDutyCycle(0.0f),
	m_zeroDutyCycle(0.0f),
	m_angleStepSize(0.0f)
{
   this->calibrate(3.1f, 13.80f); //11.75f); // Typical values
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
   logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
   get_gpio()->digitalWritePWM(0.0f);
} 

/**
 * A calibrate function that allows you to set the full useful range for the servo motor
 * using the duty cycle values. This allows you to calibrate the motor using a linear scaling
 * range.
 * @param minDutyCycle the minimum duty cycle that represents the -range value
 * @param maxDutyCycle the maximum duty cycle that represents the +range value
 * @return return 0 if successful
 */
int ServoMotor::calibrate(float minDutyCycle, float maxDutyCycle){

   if(maxDutyCycle<=minDutyCycle){
      logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Error, there is something wrong with the duty cycle values.");
      return -1;
   }
   m_minDutyCycle = minDutyCycle;
   m_maxDutyCycle = maxDutyCycle;
   m_zeroDutyCycle = (minDutyCycle + maxDutyCycle)/2.0f;
   m_angleStepSize = (maxDutyCycle - minDutyCycle) / (2 * m_plusMinusRange);

   logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Min: " + std::to_string(m_minDutyCycle) + 
	" Max: " + std::to_string(m_maxDutyCycle) +  " Zero: " + std::to_string(m_zeroDutyCycle) + 
	" Step: " + std::to_string(m_angleStepSize) + " Range: " + std::to_string(m_plusMinusRange));

   return 0;
}

/**
 * Set the servo angle using +/-angle value (zero is the center)
 * @param angle the servo angle (value must be within +/- range)
 * @return return 0 if successful
 */
void ServoMotor::setAngle(float angle){

   if((angle < (-m_plusMinusRange))||(angle > (+m_plusMinusRange))){
      logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Error, the angle selected is outsid of the servo operation range.");
      return;
   }

   float angleDutyCycle = m_zeroDutyCycle + (angle * m_angleStepSize);
   get_gpio()->digitalWritePWM(angleDutyCycle);
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
