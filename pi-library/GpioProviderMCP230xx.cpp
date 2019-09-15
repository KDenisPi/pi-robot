/*
 * GpioProviderMCP23017.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#include <fcntl.h>
#include <unistd.h>
#include <bitset>

#include "logger.h"
#include "GpioProviderMCP230xx.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "MCP230xx";

//
//
//
GpioProviderMCP230xx::GpioProviderMCP230xx(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr, const int pins) :
  GpioProvider(name, pins), _i2c(i2c), _i2caddr(i2c_addr), m_fd(-1)
{
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));
  //register I2C user
  m_fd = _i2c->add_user(name, _i2caddr);
}

//
//
//
GpioProviderMCP230xx::~GpioProviderMCP230xx() {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));
  _i2c->del_user(get_name(), m_fd);
}

/*
 *
 */
const int GpioProviderMCP230xx::dgtRead(const int pin){
    int mask = (1 << (pin & 0x07));
    int gpio = get_GPIO_addr(pin);

    int value = _i2c->I2CReadReg8 (m_fd, gpio) ;
    return ((value & mask) == 0 ? SGN_LEVEL::SGN_LOW : SGN_LEVEL::SGN_HIGH);
}

/*
 *
 */
void GpioProviderMCP230xx::dgtWrite(const int pin, const int value){
  int mask = (1 << (pin & 0x07));
  int current_mode = get_OLAT(pin);
  int gpio = get_GPIO_addr(pin);

  if(value)
    current_mode |= mask;
  else
    current_mode &= (~mask);

  _i2c->I2CWriteReg8(m_fd, gpio, current_mode);
  set_OLAT(current_mode, pin);
}
/*
*
*/
void GpioProviderMCP230xx::setmode(const int pin, const gpio::GPIO_MODE mode){
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pin: " + std::to_string(pin) + " Mode: " + std::to_string(mode));

  /*
  * PWM mode is supported for Simple provider only
  */
  if(gpio::GPIO_MODE::PWM_OUT == mode){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " PWM mode is not supported");
        throw std::runtime_error(std::string("PWM mode is not supported"));

  }

  int reg_addr = get_IODIR_addr(pin);
  int mask = (1 << (pin & 0x07));

  int current_mode  = _i2c->I2CReadReg8(m_fd, reg_addr);
  if (mode == GPIO_MODE::OUT)
    current_mode &= (~mask);
  else
    current_mode |= mask;

  _i2c->I2CWriteReg8 (m_fd, reg_addr, current_mode) ;

  std::bitset<8> bmode(current_mode);
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Mode curr: " + std::to_string(current_mode) + " Bit mask: " + bmode.to_string());
}
/*
 *
 */
void GpioProviderMCP230xx::pullUpDownControl(const int pin, const gpio::PULL_MODE pumode){
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));

  int reg_addr = get_GPPU_addr(pin);
  int mask = (1 << (pin & 0x07));

  int current_mode  = _i2c->I2CReadReg8 (m_fd, reg_addr);

  if (pumode == PULL_MODE::PULL_UP)
    current_mode |= mask ;
  else
    current_mode &= (~mask);

  _i2c->I2CWriteReg8 (m_fd, reg_addr, current_mode);

  std::bitset<8> bmode(current_mode);
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Mode curr: " + std::to_string(current_mode) +  " Bit mask: " + bmode.to_string());
}

} /* namespace gpio */
} /* namespace pirobot */
