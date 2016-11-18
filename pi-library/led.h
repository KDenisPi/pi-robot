/*
 * led.h
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_LED_H_
#define PI_LIBRARY_LED_H_

#include <memory>
#include <cassert>

#include "item.h"

namespace pirobot {
namespace item {

class Led: public Item {
public:
	enum LED_STATE{
		OFF = 0,
		ON = 1
	};

	/*
	 * Constructor
	 */
	Led(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const LED_STATE init_state = LED_STATE::OFF,
			const bool init_always=false);

	Led(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
			const LED_STATE init_state = LED_STATE::OFF,
			const bool init_always=false);

	virtual ~Led();

	virtual bool initialize();
	virtual void stop() {set_state(m_init_state);};
	virtual const std::string to_string();
	virtual const std::string printConfig();

	void On() {set_state(LED_STATE::ON);}
	void Off(){set_state(LED_STATE::OFF);}

private:

	/*
	 * Change LED state ON/OFF
	 */
	void set_state(const LED_STATE state);

	bool m_init_always;
	LED_STATE m_state; //
	LED_STATE m_init_state; //initial state
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_LED_H_ */
