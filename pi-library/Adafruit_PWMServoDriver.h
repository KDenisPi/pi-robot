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

#ifndef _ADAFRUIT_PWMServoDriver_H
#define _ADAFRUIT_PWMServoDriver_H

#include <cstdint>
#include <memory>
#include <wiringPi.h>

#include "I2C.h"
#include "provider.h"

namespace pirobot {
namespace gpio {

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1    0x00          
#define PCA9685_MODE2    0x01          
#define PCA9685_ALLCALLADR     0x05   //LED All Call i2c-bus address
#define PCA9685_PRESCALE 0xFE

#define MODE1_DISABLE_RESTART        0x80 // 0 - enable restart
#define MODE1_EXTERNAL_CLOCK         0x40 // 0 - use internal clock
#define MODE1_ENABLE_AUTO_INCREMENT  0x20 // 0 - disable auto increment
#define MODE1_ENABLE_SLEEP           0x10 // 0 - disable sleep
#define MODE1_ALLOW_SUB_ADDR1        0x08 // 0 - allow to use sub address 1
#define MODE1_ALLOW_SUB_ADDR2        0x04 // 0 - allow to use sub address 2
#define MODE1_ALLOW_SUB_ADDR3        0x02 // 0 - allow to use sub address 3
#define MODE1_ENABLE_ALL_CALL        0x01 // 0 - disable all call

#define PCA9685_MODE1_DEFAULT 0xA1          //0b10100001 - see above

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

struct LED_DATA {
  uint16_t on;
  uint16_t off;
};

class Adafruit_PWMServoDriver :  public pirobot::provider::Provider {
 public:
  Adafruit_PWMServoDriver(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr = s_i2c_addr);
  virtual ~Adafruit_PWMServoDriver();

  void begin(void);
  void reset(void);
  void sleep(void);
  void wakeup(void);

  void setFrequency(float freq);
  float getFrequency();

  void setPWM(uint8_t num, uint16_t on, uint16_t off);
  void setPin(uint8_t num, uint16_t val, bool invert=false);
  void getPin(uint8_t num, LED_DATA& data);
  const uint8_t getPrescale() const {return m_prescale;}

  virtual const std::string printConfig() override {
        return get_name() + " I2C addr: " + std::to_string(_i2caddr) + "\n";
  }

  static const uint8_t s_i2c_addr;

 private:
  uint8_t _i2caddr;
  int m_fd;
  uint8_t m_prescale;

  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t d);
};

} /* namespace gpio */
} /* namespace pirobot */

#endif
