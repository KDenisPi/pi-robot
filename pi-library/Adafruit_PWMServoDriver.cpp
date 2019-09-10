/***************************************************
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <math.h>
#include <algorithm>

#include "timers.h"
#include "Adafruit_PWMServoDriver.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "PWM";

const uint8_t Adafruit_PWMServoDriver::s_i2c_addr = 0x40;

#define CALIB_FACTOR   0.89   //the calibration factor -- manually calculated

// Set to true to print some debug messages, or false to disable them.
#define ENABLE_DEBUG_OUTPUT false

Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(const std::string& name,
    std::shared_ptr<pirobot::i2c::I2C> i2c,
    const uint8_t i2c_addr) : Provider(pirobot::provider::PROVIDER_TYPE::PROV_PWM, name), m_prescale(0), _i2c(i2c), _i2caddr(i2c_addr) {

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Name: " + name + " Addr: " + std::to_string(i2c_addr));

  //register I2C user
  _i2c->add_user(name, _i2caddr);
  m_fd = _i2c->I2CSetup(_i2caddr);
}

//
//
//
Adafruit_PWMServoDriver::~Adafruit_PWMServoDriver() {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));
}


//
//
//
void Adafruit_PWMServoDriver::begin(void) {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
  reset();
}

void Adafruit_PWMServoDriver::reset(void) {
 write8((uint8_t)PCA9685_MODE1, (uint8_t)PCA9685_MODE1_DEFAULT);
}

void Adafruit_PWMServoDriver::wakeup(void) {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Wake Up");
  reset();
}

void Adafruit_PWMServoDriver::sleep(void) {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set sleep mode");
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode | MODE1_ENABLE_SLEEP); // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
}

/**
 * This method gets the frequency that is currently stored in the PCA9685 device
 * @return returns the frequency that is stored (between 24Hz and 1526Hz)
 */
float Adafruit_PWMServoDriver::getFrequency(){
   int value = (int) read8(PCA9685_PRESCALE); // get the prescaler
   float frequency = (25000000.0f / (4096.0f * (value + 1)));
   return (frequency / CALIB_FACTOR);
}

/*
 *
 */
void Adafruit_PWMServoDriver::setFrequency(float freq) {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Freq: " + std::to_string(freq));

  freq *= CALIB_FACTOR;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  m_prescale = floor(prescaleval + 0.5);

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & 0x7F) | MODE1_ENABLE_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, m_prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);

  piutils::timers::Timers::delay(5); //milliseconds

  write8(PCA9685_MODE1, oldmode | PCA9685_MODE1_DEFAULT); //  This sets the MODE1 register to turn on auto increment.
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  // This is why the beginTransmission below was not working.
}

void Adafruit_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) +
	" Pin: " + std::to_string(num) +
        " On: " + std::to_string(on) +
        " Off: " + std::to_string(off));

  int offset = 4*num;
  write8(LED0_ON_L + offset, on);
  write8(LED0_ON_H + offset, on>>8);
  write8(LED0_OFF_L + offset, off);
  write8(LED0_OFF_H + offset, off>>8);
}

// Sets pin without having to deal with on/off tick placement and properly handles
// a zero value as completely off.  Optional invert parameter supports inverting
// the pulse for sinking to ground.  Val should be a value from 0 to 4095 inclusive.
void Adafruit_PWMServoDriver::setPin(uint8_t num, uint16_t val, bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  const uint16_t vmax = 4095;
  val = std::min(val, vmax);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == vmax) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, vmax-val);
    }
  }
  else {
    if (val == vmax) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
  }
}

/*
 *
 */
void Adafruit_PWMServoDriver::getPin(uint8_t num, LED_DATA& data){
	  int offset = 4*num;
	  uint16_t low,  high;

	  low = read8(LED0_ON_L + offset);
	  high = read8(LED0_ON_H + offset);
	  data.on = (high<<8) | low;

	  low = read8(LED0_OFF_L + offset);
	  high = read8(LED0_OFF_H + offset);
	  data.off = (high<<8) | low;
}


uint8_t Adafruit_PWMServoDriver::read8(uint8_t addr) {

  _i2c->lock();
  uint8_t result = _i2c->I2CReadReg8(m_fd, addr);
  return result;
}

void Adafruit_PWMServoDriver::write8(uint8_t addr, uint8_t d){
  _i2c->lock();
  _i2c->I2CWriteReg8(m_fd, addr, d);
}

} /* namespace gpio */
} /* namespace pirobot */

