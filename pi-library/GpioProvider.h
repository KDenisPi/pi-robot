/*
 * GpioProvider.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_GPIOPROVIDER_H_
#define PI_LIBRARY_GPIOPROVIDER_H_

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
	GpioProvider(const int pstart=0) :
		m_pstart(pstart)
	{}

	virtual ~GpioProvider() {}

	const int getStartPin() { return m_pstart; }
	const int getRealPin(const int pin) { return pin - m_pstart; }

	virtual const std::string to_string() = 0;
	virtual const int dgtRead(const int pin) = 0;
	virtual void dgtWrite(const int pin, const int value) = 0;
	virtual void setmode(const int pin, const gpio::GPIO_MODE mode) = 0;
	virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) = 0;

private:
	int m_pstart; //number of the first GPIO pin assigned for this provider. Real pin number (pin - pstart)

};


}
}
#endif /* PI_LIBRARY_GPIOPROVIDER_H_ */
