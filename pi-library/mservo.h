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
	ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const uint16_t offset = 150,
			const uint16_t max = 425);

	ServoMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
			const uint16_t offset = 150,
			const uint16_t max = 425);

	virtual ~ServoMotor();

        virtual bool initialize() override;
        virtual void stop() override;

        virtual const std::string to_string() override;
        virtual const std::string printConfig() override;

	const uint16_t get_max() {return m_max;}
	const uint16_t get_offset() {return m_offset;}
	const uint16_t get_curent();
	const uint16_t set_position(const uint16_t pos);

	void setPulse(const uint16_t pulselen);
private:
	uint16_t m_max;    //maxumum value for servo motor (different from model to model)
	uint16_t m_offset; //offset value used for PWM set

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_MSERVO_H_ */
