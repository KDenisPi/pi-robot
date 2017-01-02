/*
 * TiltSwitch.h
 *
 *  Created on: Jan 1, 2017
 *      Author: denis
 */

#ifndef PI_LIBRARY_TILTSWITCH_H_
#define PI_LIBRARY_TILTSWITCH_H_

#include "button.h"

namespace pirobot {
namespace item {

class TiltSwitch: public Button {
public:
	TiltSwitch(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_UP,
			const int itype = ItemTypes::TILT_SWITCH);

	TiltSwitch(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
            const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
			const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_UP,
			const int itype = ItemTypes::TILT_SWITCH);

	virtual ~TiltSwitch();
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_TILTSWITCH_H_ */
