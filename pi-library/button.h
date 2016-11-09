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
#include "gpio.h"

namespace pirobot {
namespace item {

class Button: public Item {
public:
	enum BUTTON_STATE{
		DISCONNECTED = 0,
		CONNECTED = 1
	};

	/*
	 * Constructor
	 */
	Button(const std::shared_ptr<gpio::Gpio> gpio,
			const BUTTON_STATE state = BUTTON_STATE::DISCONNECTED);

	Button(const std::shared_ptr<gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
                        const BUTTON_STATE state = BUTTON_STATE::DISCONNECTED);

	virtual ~Button();
	virtual bool initialize();
	virtual const std::string to_string();

	static void* worker(void* p);
	void stop() { m_stop = true;}
	bool is_stopped() { return m_stop;}

private:
	std::shared_ptr<gpio::Gpio> m_gpio;
	BUTTON_STATE m_state; //
	pthread_t m_pthread;
	bool m_stop;
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_BUTTON_H_ */
