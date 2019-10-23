/*
 * ServoMotor.h
 *
 *  Created on: Nov 16, 2016
 *      Author: Denis Kudia
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
            const std::string name,
            const std::string comment,
            const float plusMinusRange = 90.0f);

    virtual ~ServoMotor();

    virtual bool initialize() override;
    virtual void stop() override;

    virtual const std::string to_string() override;
    virtual const std::string printConfig() override;

    virtual int calibrate(float minDutyCycle, float maxDutyCycle);
    virtual void setAngle(float angle);

    void setPulse(const uint16_t pulselen);
private:
    float m_minDutyCycle, m_maxDutyCycle, m_zeroDutyCycle;  ///< The calibrated duty cycles
    float m_plusMinusRange;                             ///< The range of the servo (e.g., +/- 90)
    float m_angleStepSize;                              ///< Calculated from other duty cycles and range
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_MSERVO_H_ */
