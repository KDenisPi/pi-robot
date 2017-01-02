/*
 * TiltSwitch.cpp
 *
 *  Created on: Jan 1, 2017
 *      Author: denis
 */

#include "TiltSwitch.h"
#include "logger.h"

namespace pirobot {
namespace item {

const char TAG[] = "tilt";


TiltSwitch::TiltSwitch(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const BUTTON_STATE state,
		const gpio::PULL_MODE pullmode,
		const int itype) :
				Button(gpio, state, pullmode, itype){

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");
}

TiltSwitch::TiltSwitch(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,
		const std::string comment,
        const BUTTON_STATE state,
		const gpio::PULL_MODE pullmode,
		const int itype) :
			Button(gpio, name, comment, state, pullmode, itype){

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");
}


TiltSwitch::~TiltSwitch() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");
}

} /* namespace item */
} /* namespace pirobot */
