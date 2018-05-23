/*
 * GpioProviderMCP23017.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#include <fcntl.h>
#include <unistd.h>
#include <bitset>

#include "I2CWrapper.h"
#include "GpioProviderMCP23017.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "MCP23017";

const int GpioProviderMCP23017::s_pins = 16;
const uint8_t GpioProviderMCP23017::s_i2c_addr = 0x20;

//
//
//
GpioProviderMCP230xx::GpioProviderMCP230xx(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr, const int pins) :
  GpioProvider(name, pins), _i2caddr(i2c_addr), m_fd(-1)
{
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

  //register I2C user
  i2c->add_user(name, _i2caddr);
}

//
//
//
GpioProviderMCP230xx::~GpioProviderMCP230xx() {
  if(m_fd != -1){
    I2CWrapper::lock();
    close(m_fd);
    m_fd = -1;
    I2CWrapper::unlock();
  }
}

/*
 *
 */
const int GpioProviderMCP230xx::dgtRead(const int pin){
    int mask = (1 << (pin & 0x07));
    int gpio = get_GPIO_addr(pin);

    I2CWrapper::lock();
    int value = I2CWrapper::I2CReadReg8 (m_fd, gpio) ;
    I2CWrapper::unlock();

    return ((value & mask) == 0 ? SGN_LEVEL::SGN_LOW : SGN_LEVEL::SGN_HIGH);
}

/*
 *
 */
void GpioProviderMCP230xx::dgtWrite(const int pin, const int value){
  int mask = (1 << (pin & 0x07));
  int current_mode = get_OLAT(pin);
  int gpio = get_GPIO_addr(pin);

  I2CWrapper::lock();
  I2CWrapper::I2CWriteReg8(m_fd, gpio, current_mode);
  I2CWrapper::unlock();

  set_OLAT(pin, current_mode);
}
/*
*
*/
void GpioProviderMCP230xx::setmode(const int pin, const gpio::GPIO_MODE mode){
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pin: " + std::to_string(pin) + " Mode: " + std::to_string(mode));

  int reg_addr = get_IODIR_addr(pin);
  int mask = (1 << (pin & 0x07));

  I2CWrapper::lock();

  int current_mode  = I2CWrapper::I2CReadReg8(m_fd, reg_addr);
  if (mode == GPIO_MODE::OUT)
    current_mode &= (~mask);
  else
    current_mode |= mask;

  I2CWrapper::I2CWriteReg8 (m_fd, reg_addr, current_mode) ;
  I2CWrapper::unlock();

  std::bitset<8> bmode(current_mode);
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Mode curr: " + std::to_string(current_mode) +
        " Bit mask: " + bmode.to_string());
}
/*
 *
 */
void GpioProviderMCP230xx::pullUpDownControl(const int pin, const gpio::PULL_MODE pumode){
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));

  int reg_addr = get_GPPU_addr(pin);
  int mask = (1 << (pin & 0x07));

  I2CWrapper::lock();
  int current_mode  = I2CWrapper::I2CReadReg8 (m_fd, reg_addr);

  if (pumode == PULL_MODE::PULL_UP)
    current_mode |= mask ;
  else
    current_mode &= (~mask);

  I2CWrapper::I2CWriteReg8 (m_fd, reg_addr, current_mode);
  I2CWrapper::unlock();

  std::bitset<8> bmode(current_mode);
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Mode curr: " + std::to_string(current_mode) +
        " Bit mask: " + bmode.to_string());
}

//
//
//
GpioProviderMCP23017::GpioProviderMCP23017(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr, const int pins) :
  GpioProviderMCP230xx(name, i2c, i2c_addr, pins), m_OLATA(0), m_OLATB(0)
{
  //
  // Confifure device and set initial values
  //

  I2CWrapper::lock();
  m_fd = I2CWrapper::I2CSetup(_i2caddr);

  I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_IOCON, 0x3A);
  I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_IOCONB, 0x3A);

  I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_IODIRA, 0x00);
  I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_IODIRB, 0x00);

  m_OLATA = I2CWrapper::I2CReadReg8 (m_fd, MCP23x17_GPIOA);
  m_OLATB = I2CWrapper::I2CReadReg8 (m_fd, MCP23x17_GPIOB);

  I2CWrapper::unlock();

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ---> OLATA: " + std::to_string(m_OLATA));
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ---> OLATB: " + std::to_string(m_OLATB));
}

//
//
//
GpioProviderMCP23017::~GpioProviderMCP23017() {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started " + this->to_string());
}

} /* namespace gpio */
} /* namespace pirobot */
