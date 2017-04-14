/*
 * button.h
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_BUTTON_H_
#define PI_LIBRARY_BUTTON_H_

#include <memory>
#include <cassert>

#include "item.h"
#include "Threaded.h"

namespace pirobot {
namespace item {


class Button: public Item, public Threaded {
public:

	/*
	 * Constructor
	 */
	Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_DOWN, //gpio::PULL_MODE::PULL_UP,
			const int itype = ItemTypes::BUTTON);

	Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
            const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_DOWN, //gpio::PULL_MODE::PULL_UP,
			const int itype = ItemTypes::BUTTON);

	virtual ~Button();
	virtual bool initialize() override;
	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;

	static void* worker(void* p);
	virtual void stop() override;

	void set_state(const BUTTON_STATE state);
	const BUTTON_STATE state() { return m_state; }

private:
	gpio::PULL_MODE m_pullmode;
	BUTTON_STATE m_state; //
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_BUTTON_H_ */
