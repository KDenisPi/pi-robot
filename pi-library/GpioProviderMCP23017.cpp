/*
 * GpioProviderMCP23017.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: denis
 */

#include <mcp23x0817.h>

#include "I2CWrapper.h"
#include "GpioProviderMCP23017.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "MCP23017";

GpioProviderMCP23017::GpioProviderMCP23017(const uint8_t i2caddr) :
		GpioProvider(50, 16), _i2caddr(i2caddr), m_OLATA(0), m_OLATB(0)
{
	  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

	  I2CWrapper::lock();

	  m_fd = I2CWrapper::I2CSetup(_i2caddr);
	  I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_IOCON, IOCON_INIT);

	  m_OLATA = I2CWrapper::I2CReadReg8 (m_fd, MCP23x17_OLATA) ;
	  m_OLATB = I2CWrapper::I2CReadReg8 (m_fd, MCP23x17_OLATB) ;

	  I2CWrapper::unlock();
}

GpioProviderMCP23017::~GpioProviderMCP23017() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
const std::string GpioProviderMCP23017::to_string(){
	return std::string("MCP23017")+ " From: " + std::to_string(getStartPin());
}

/*
 *
 */
const int GpioProviderMCP23017::dgtRead(const int pin){
	  int mask, value, gpio ;
	  int rpin = getRealPin(pin);

	  if (rpin < 8){          // Bank A
	    gpio  = MCP23x17_GPIOA ;
	    mask  = 1 << rpin ;
	  }
	  else
	  {
	    gpio  = MCP23x17_GPIOB ;
	    mask = 1 << (rpin & 0x07) ;
	  }

	  I2CWrapper::lock();
	  value = I2CWrapper::I2CReadReg8 (m_fd, gpio) ;
	  I2CWrapper::unlock();

	  if ((value & mask) == 0)
	    return SGN_LEVEL::SGN_LOW ;
	  else
	    return SGN_LEVEL::SGN_HIGH ;
}

/*
 *
 */
void GpioProviderMCP23017::dgtWrite(const int pin, const int value){

	  int bit, current_mode ;
	  int rpin = getRealPin(pin);	// Pin now 0-15
	  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pin: " + std::to_string(pin) + 
			" Real: " + std::to_string(rpin) + " Value: " + std::to_string(value));

	  bit = 1 << (rpin & 0x07) ;

	  if(rpin < 8)			// Bank A
	  {
		current_mode = m_OLATA;

		if(value == SGN_LEVEL::SGN_LOW)
	    	current_mode &= (~bit) ;
	    else
	    	current_mode |=   bit ;

			I2CWrapper::lock();
			I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_GPIOA, current_mode);
			I2CWrapper::unlock();

			m_OLATA = current_mode ;
	  }
	  else				// Bank B
	  {
		current_mode = m_OLATB;

		if(value == SGN_LEVEL::SGN_LOW)
	    	current_mode &= (~bit) ;
	    else
	    	current_mode |=   bit ;

		  	I2CWrapper::lock();
		  	I2CWrapper::I2CWriteReg8(m_fd, MCP23x17_GPIOB, current_mode);
		  	I2CWrapper::unlock();

		  	m_OLATB = current_mode ;
	  }
}

/*
 *
 */
void GpioProviderMCP23017::setmode(const int pin, const gpio::GPIO_MODE mode){
	int reg_addr, mask, current_mode;
	int rpin = getRealPin(pin);

  	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pin: " + std::to_string(pin) + 
			" Real: " + std::to_string(rpin) + " Mode: " + std::to_string(mode));

	if (rpin < 8)          // Bank A
		reg_addr  = MCP23x17_IODIRA ;
	else
	{
		reg_addr  = MCP23x17_IODIRB ;
		rpin &= 0x07 ;
	}

	mask = 1 << rpin ;

  	I2CWrapper::lock();
	current_mode  = I2CWrapper::I2CReadReg8 (m_fd, reg_addr);
  	I2CWrapper::unlock();


	if (mode == GPIO_MODE::OUT)
		current_mode &= (~mask) ;
	else
		current_mode |=   mask ;

  	I2CWrapper::lock();
	I2CWrapper::I2CWriteReg8 (m_fd, reg_addr, current_mode) ;
  	I2CWrapper::unlock();
}

/*
 *
 */
void GpioProviderMCP23017::pullUpDownControl(const int pin, const gpio::PULL_MODE pumode){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") +
			std::to_string(pin) + " UP mode: " + std::to_string(pumode));

	int mask, current_mode, reg_addr ;
	int rpin = getRealPin(pin);

	if (rpin < 8)		// Bank A
		reg_addr  = MCP23x17_GPPUA ;
	else
	{
		reg_addr  = MCP23x17_GPPUB ;
	    rpin &= 0x07 ;
	}

	mask = 1 << pin ;
	current_mode  = wiringPiI2CReadReg8 (m_fd, reg_addr) ;

	if (pumode == PULL_MODE::PULL_UP)
	  current_mode |=   mask ;
	else
	  current_mode &= (~mask) ;

	wiringPiI2CWriteReg8 (m_fd, reg_addr, current_mode) ;
}


} /* namespace gpio */
} /* namespace pirobot */
