/*
 * GpioProvider.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDER_H_
#define PI_LIBRARY_GPIOPROVIDER_H_

#include <cassert>
#include <string>

#include "provider.h"

namespace pirobot{
namespace gpio {

enum GPIO_MODE {
	IN 	= 0x0,  // 0b000
	OUT 	= 0x1,	// 0b001
	ALT5 	= 0x2,	// 0b010
	ALT4	= 0x3,  // 0b011
	ALT0    = 0x4,  // 0b100
	ALT1    = 0x5,  // 0b101
	ALT2    = 0x6,  // 0b110
	ALT3	= 0x7,	// 0b111
	PWM_OUT = 0xF	//fake value, replaced by real one for appropriate GPIO
};

// Pull up/down/none
enum PULL_MODE {
    PULL_OFF = 0,
    PULL_DOWN = 1,
    PULL_UP = 2
};

// Signal level HIGH/LOW
enum SGN_LEVEL {
    SGN_LOW = 0,
    SGN_HIGH = 1
};

enum GPIO_PROVIDER_TYPE {
        PROV_FAKE,     //Fake provider for test purpose only
        PROV_SIMPLE,   // Rasberry Pi
        PROV_PCA9685,  // PCA 9685
        PROV_MCP23017, // MCP 23017
        PROV_MCP23008  // MCP 23008
};

class GpioProvider : public pirobot::provider::Provider {
public:
	GpioProvider(const std::string& name, const int pin_count = 0, const std::string comment = "") :
		Provider(pirobot::provider::PROVIDER_TYPE::PROV_GPIO, name, comment), m_pcount(pin_count)
	{}

	virtual ~GpioProvider() {}
    virtual GPIO_PROVIDER_TYPE const get_type() const = 0;

	/*
	 *
	 */
	int getRealPin(const int pin) const noexcept(false){
		assert(pin < m_pcount);
		if(pin >= m_pcount){
			throw std::runtime_error(std::string("Invalid Pin number"));
		}
		return pin;
	}

	/*
	 *
	 */
	bool is_valid_pin(int pin) const {
		assert(pin < m_pcount);
		return (pin < m_pcount);
	}

	virtual const std::string to_string() = 0;
	virtual const int dgtRead(const int pin) = 0;
	virtual void dgtWrite(const int pin, const int value) = 0;
	virtual void dgtWritePWM(const int pin, const float dutyCycle, const float phaseOffset) {};
	virtual void setmode(const int pin, const gpio::GPIO_MODE mode) noexcept(false) = 0;
	virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) {};
	virtual void setPulse(const int pin, const uint16_t pulselen) {};

private:
	int m_pcount;
};

} //namespace gpio
} //namespace pirobot
#endif /* PI_LIBRARY_GPIOPROVIDER_H_ */
