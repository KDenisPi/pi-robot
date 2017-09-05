/*
 * ULN2003StepperMotor.h
 *
 *  Created on: Jan 18, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_ULN2003STEPPERMOTOR_H_
#define PI_LIBRARY_ULN2003STEPPERMOTOR_H_

#include "Threaded.h"
#include "item.h"
#include "motor.h"

namespace pirobot {
namespace item {

class ULN2003StepperMotor: public Item, public piutils::Threaded, public Motor {
public:
    ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_3);

    ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
            const std::shared_ptr<pirobot::gpio::Gpio> gpio_3,
            const std::string name,
            const std::string comment);

    virtual ~ULN2003StepperMotor();

    void set_direction(const MOTOR_DIR direction);

    //change direction to opposite
    void set_revers_direction(){
        set_direction( (m_direction == MOTOR_DIR::DIR_CLOCKWISE ? MOTOR_DIR::DIR_COUTERCLOCKWISE : MOTOR_DIR::DIR_CLOCKWISE));
    }

    inline const MOTOR_DIR direction() const {
        return m_direction;
    }

    virtual bool initialize() override;
    virtual const std::string to_string() override;
    virtual const std::string printConfig() override;

    virtual void stop() override;


    /*
    * Worker function
    */
    static void worker(ULN2003StepperMotor* p);

    const int get_steps() const {
        return m_num_steps;
    }

    bool start(){
        return piutils::Threaded::start<pirobot::item::ULN2003StepperMotor>(this);
    }
    
    void set_steps(const int num_steps);

    const uint8_t get_current_step() const {
        return m_step;
    }

    void save_current_step(const uint8_t step){
        m_step = step;
    }

private:
    int m_num_steps;
    uint8_t m_step;
    MOTOR_DIR m_direction;

    uint8_t m_cmd[8]  = {0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x01};
    
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_1;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_2;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_3;

public:    
    /*
     *
     */
    inline const uint8_t get_next_step(const uint8_t step){
        if(m_direction == MOTOR_DIR::DIR_CLOCKWISE)
            return (step == 7 ? 0 : step+1);

        return (step == 0 ? 7 : step-1);
    }
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_ULN2003STEPPERMOTOR_H_ */
