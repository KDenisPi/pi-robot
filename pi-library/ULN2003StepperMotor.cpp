/*
 * ULN2003StepperMotor.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: Denis Kudia
 */

const char TAG[] = "ST_U2003";

#include <thread>

#include "ULN2003StepperMotor.h"
#include "logger.h"

#include <wiringPi.h>

namespace pirobot {
namespace item {

ULN2003StepperMotor::ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_3) :
            Item(gpio_0, ItemTypes::STEPPER),
            m_gpio_1(gpio_1),
            m_gpio_2(gpio_2),
            m_gpio_3(gpio_3),
            m_step(10),
            m_direction(MOTOR_DIR::DIR_CLOCKWISE)
{
    assert(get_gpio() != NULL);
    assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

    assert(m_gpio_1.get() != NULL);
    assert(m_gpio_1->getMode() ==  gpio::GPIO_MODE::OUT);
    assert(m_gpio_2.get() != NULL);
    assert(m_gpio_2->getMode() ==  gpio::GPIO_MODE::OUT);
    assert(m_gpio_3.get() != NULL);
    assert(m_gpio_3->getMode() ==  gpio::GPIO_MODE::OUT);

}

ULN2003StepperMotor::ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio_3,
        const std::string name,
        const std::string comment)	:
            Item(gpio_0, name, comment, ItemTypes::STEPPER),
            m_gpio_1(gpio_1),
            m_gpio_2(gpio_2),
            m_gpio_3(gpio_3),
            m_step(10),
            m_direction(MOTOR_DIR::DIR_CLOCKWISE)
{
    assert(get_gpio() != NULL);
    assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

    assert(m_gpio_1.get() != NULL);
    assert(m_gpio_1->getMode() ==  gpio::GPIO_MODE::OUT);
    assert(m_gpio_2.get() != NULL);
    assert(m_gpio_2->getMode() ==  gpio::GPIO_MODE::OUT);
    assert(m_gpio_3.get() != NULL);
    assert(m_gpio_3->getMode() ==  gpio::GPIO_MODE::OUT);

}

/*
 *
 */
ULN2003StepperMotor::~ULN2003StepperMotor() {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    stop();
}

/*
*
*/
void ULN2003StepperMotor::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    piutils::Threaded::stop();
    
    this->get_gpio()->set_level( pirobot::gpio::SGN_LEVEL::SGN_LOW );
    this->m_gpio_1->set_level( pirobot::gpio::SGN_LEVEL::SGN_LOW );
    this->m_gpio_2->set_level( pirobot::gpio::SGN_LEVEL::SGN_LOW );
    this->m_gpio_3->set_level( pirobot::gpio::SGN_LEVEL::SGN_LOW );
}

/*
 *
 */
void ULN2003StepperMotor::set_direction(const MOTOR_DIR direction){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Direction: " + std::to_string(direction));
    m_direction = direction;
}

/*
 *
 */
 void ULN2003StepperMotor::worker(ULN2003StepperMotor* owner){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started.");
    std::string name = owner->name();
    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, [owner]{return owner->is_stop_signal() || owner->get_steps() > 0;});
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker. Signal detected.");
        }

        if(owner->is_stop_signal())
            break;

        auto fstep = [](ULN2003StepperMotor* owner, const uint8_t step) -> void {
            owner->get_gpio()->set_level( ((step & 0x01) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
            owner->m_gpio_1->set_level( ((step & 0x02) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
            owner->m_gpio_2->set_level( ((step & 0x04) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
            owner->m_gpio_3->set_level( ((step & 0x08) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
        };

        int steps = owner->get_steps();
        uint8_t step = owner->get_current_step();
        for(int i = 0; i < steps && !owner->is_stop_signal() && owner->is_rotate(); i++){
            fstep(owner, step);
            step = owner->get_next_step(step);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        /*
        * If there was not stop by external reason send notification - Done
        */
        bool external_stop = (owner->is_stop_signal() || !owner->is_rotate());
        if(!external_stop && owner->notify){
            unsigned int state = GENERAL_NTFY::GN_DONE;
            owner->notify(owner->type(), name, (void*)(&state));
         }
   
        owner->save_current_step(step);

        /*
        *TODO: Send notification that cycle finished
        */
    
        owner->set_steps(0);
    }

 }

/*
 *
 */
bool ULN2003StepperMotor::initialize(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started ");
    m_step = (m_direction == MOTOR_DIR::DIR_CLOCKWISE ? 0 : 7);
    return true;
}

/*
 *
 */
const std::string ULN2003StepperMotor::to_string(){
    return name();
}

/*
 *
 */
const std::string ULN2003StepperMotor::printConfig(){
    std::string conf =  name() + "\n" +
        " GPIO Blue (1): " + get_gpio()->to_string() + "\n";
    conf += " GPIO Pink (2): " + (m_gpio_1.get() == nullptr ? "Not Defined" : m_gpio_1->to_string()) + "\n";
    conf += " GPIO Yell (3): " + (m_gpio_2.get() == nullptr ? "Not Defined" : m_gpio_2->to_string()) + "\n";
    conf += " GPIO Orng (4): " + (m_gpio_3.get() == nullptr ? "Not Defined" : m_gpio_3->to_string()) + "\n";
    conf = conf +" Direction: " + std::to_string(m_direction);
    return conf;
}



} /* namespace item */
} /* namespace pirobot */
