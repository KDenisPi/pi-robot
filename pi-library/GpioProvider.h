/*
 * GpioProvider.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDER_H_
#define PI_LIBRARY_GPIOPROVIDER_H_

#include <cassert>
#include <string>

namespace pirobot{
namespace gpio {

enum GPIO_MODE {
	IN = 0,
	OUT = 1
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


class GpioProvider {
public:
	GpioProvider(const int pin_start=0, const int pin_count = 0) :
		m_pstart(pin_start), m_pcount(pin_count)
	{}

	virtual ~GpioProvider() {}

	int getStartPin() const {return m_pstart;}

	/*
	 *
	 */
	int getRealPin(const int pin) const {
		assert((pin-m_pstart) < m_pcount);
		return pin - m_pstart;
	}

	/*
	 *
	 */
	bool validate_pin(int pin) const {
		assert(pin >= m_pstart);
		assert(pin < (m_pstart+m_pcount));

		if((pin >= m_pstart) && (pin < (m_pstart+m_pcount)))
			return true;

		return false;
	}

	virtual const std::string to_string() = 0;
	virtual const int dgtRead(const int pin) = 0;
	virtual void dgtWrite(const int pin, const int value) = 0;
	virtual void dgtWritePWM(const int pin, const float dutyCycle, const float phaseOffset) {};
	virtual void setmode(const int pin, const gpio::GPIO_MODE mode) = 0;
	virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) {};
	virtual void setPulse(const int pin, const uint16_t pulselen) {};


private:
	int m_pstart; //number of the first GPIO pin assigned for this provider. Real pin number (pin - pstart)
	int m_pcount;

};


}
}
#endif /* PI_LIBRARY_GPIOPROVIDER_H_ */
