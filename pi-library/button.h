/*
 * button.h
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_BUTTON_H_
#define PI_LIBRARY_BUTTON_H_

#include <memory>
#include <cassert>

#include <pthread.h>

#include "item.h"

namespace pirobot {
namespace item {

class Button: public Item {
public:
	enum BUTTON_STATE{
		BTN_NOT_PUSHED = 0,
		BTN_PUSHED = 1
	};

	/*
	 * Constructor
	 */
	Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_UP);

	Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
            const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_UP);

	virtual ~Button();
	virtual bool initialize();
	virtual const std::string to_string();
	virtual const std::string printConfig();

	static void* worker(void* p);
	virtual void stop();

	bool is_stopped() { return (m_pthread == 0);}
	bool is_stopSignal() {  return m_stopSignal; }

	void set_state(const BUTTON_STATE state);
	const BUTTON_STATE state() { return m_state; }

private:
	//std::shared_ptr<gpio::Gpio> m_gpio;

	gpio::PULL_MODE m_pullmode;
	BUTTON_STATE m_state; //

	pthread_t m_pthread;
	bool m_stopSignal;
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_BUTTON_H_ */
