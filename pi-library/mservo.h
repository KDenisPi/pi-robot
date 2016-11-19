/*
 * ServoMotor.h
 *
 *  Created on: Nov 16, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_MSERVO_H_
#define PI_LIBRARY_MSERVO_H_

#include <memory>
#include "item.h"

namespace pirobot {
namespace item {

class ServoMotor: public Item {
public:
	ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio);

	ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment);

	virtual ~ServoMotor();

        virtual bool initialize() override;
        virtual void stop() override;

        virtual const std::string to_string() override;
        virtual const std::string printConfig() override;

	void setPulse(const uint16_t pulselen);

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_MSERVO_H_ */
