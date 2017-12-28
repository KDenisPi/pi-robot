/*
 * PiRobot.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#include <unistd.h>
#include <iostream>
#include <functional>
#include <iterator>

#include "PiRobot.h"
#include "logger.h"

#include "GpioProviderFake.h"
#include "GpioProviderSimple.h"
#include "GpioProviderMCP23017.h"
#include "GpioProviderPCA9685.h"
#include "SPI.h"
#include "I2C.h"

#include "item.h"
#include "led.h"
#include "button.h"
#include "blinking.h"
#include "Drv8835.h"
#include "DCMotor.h"
#include "ULN2003StepperMotor.h"
#include "MCP320X.h"
#include "AnalogLightMeter.h"

namespace pirobot {

const char TAG[] = "PiRobot";

PiRobot::PiRobot(const bool realWorld)
    : m_realWorld(realWorld),
      stm_notification(nullptr)
{
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + std::string("Started. Real world? ") + 
        (realWorld ? " TRUE" : " FALSE"));

}

PiRobot::~PiRobot() {
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + "Started");
    /*
     * Delete all objects
     */
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + "Finished");
}

/*
 * Get GPIO by ID
 */
std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const std::string& name) const{
    auto pgpio = this->gpios.find(name);
    if(pgpio == gpios.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Absent requested GPIO with ID " + name);
        throw std::runtime_error(std::string("No GPIO with ID: ") + name);
    }
    return pgpio->second;
}

/*
 * Get GPIO by provider name and PIN number
 */
 std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const std::string provider_name, const int pin)  const{
    auto name = get_gpio_name(provider_name, pin);
    return get_gpio(name);
}


/*
 * Get Item by name
 */
std::shared_ptr<item::Item> PiRobot::get_item(const std::string& name) const{
    auto item = this->items.find(name);
    if(item == items.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Absent requested Item with ID " + name);
        throw std::runtime_error(std::string("No Item with ID: ") + name);
    }
    return item->second;
}

/*
* Get provider by name
*/
std::shared_ptr<provider::Provider> PiRobot::get_provider(const std::string& name) const{
    auto provider = this->providers.find(name);
    if(provider == providers.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Absent requested Provider with ID " + name);
        throw std::runtime_error(std::string("No Provider with ID: ") + name);
    }
    return provider->second;
}

/*
* Create GPIO for the provider. Provider should created before using add_provider function
*/
void PiRobot::add_gpio(const std::string& name, 
    const std::string& provider_name, 
    const pirobot::gpio::GPIO_MODE gpio_mode, 
    const int pin) noexcept(false){

    const auto provider = get_provider(provider_name);
    if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid provider type");
        throw std::runtime_error(std::string("Invalid provider type"));
    }
    auto gpio_provider = std::static_pointer_cast<pirobot::gpio::GpioProvider>(provider);
    // Validate pin number
    if(!gpio_provider->is_valid_pin(pin)){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid pin number: " + std::to_string(pin));
        throw std::runtime_error(std::string(" Invalid pin number: ") + std::to_string(pin));
    }

    auto gpio_name = (name.empty() ? get_gpio_name(provider_name, pin) : name);

    auto pgpio = this->gpios.find(gpio_name);
    if(pgpio != gpios.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO with such name is present already: " + gpio_name + " Provider: " + provider_name);
        throw std::runtime_error(std::string(" GPIO with such name is present already: ") + gpio_name);
    }
 
    gpios_add(gpio_name,
            std::shared_ptr<pirobot::gpio::Gpio>(
                new pirobot::gpio::Gpio(pin, gpio_mode, gpio_provider)
            ));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Provider: " + provider_name + ". Added PIN: " + std::to_string(pin));
}

/*
*
*/
void PiRobot::add_item(const pirobot::item::ItemConfig& iconfig){
    
    auto f_gpio_validation = [](const pirobot::item::ItemConfig& iconfig)->bool {
        if(iconfig.type == item::ItemTypes::UNKNOWN || iconfig.name.empty())
            return false;

        if(iconfig.gpios.size() == 0)
            return false;

        return true;
    };

    auto f_general_validation = [](const pirobot::item::ItemConfig& iconfig)->bool {
        if(iconfig.type == item::ItemTypes::UNKNOWN || iconfig.name.empty())
            return false;

        return true;    
    };

    auto f_get_gpio_provider_name = [this](const pirobot::item::ItemConfig& iconfig, int idx) -> std::string{

        const auto gpio = iconfig.gpios[idx];
        const auto provider = get_provider(gpio.first);
        if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid provider type");
            throw std::runtime_error(std::string("Invalid provider type"));
        }

        auto gpio_provider = std::static_pointer_cast<pirobot::gpio::GpioProvider>(provider);

        return get_gpio_name(gpio_provider->get_name(), gpio.second);
    };

    switch(iconfig.type){
        case item::ItemTypes::LED:
        case item::ItemTypes::BUTTON:
        case item::ItemTypes::DRV8835:
        case item::ItemTypes::DCMotor:
        case item::ItemTypes::ULN2003Stepper:
        case item::ItemTypes::AnlgDgtConvertor:
        {
                if(!f_gpio_validation(iconfig)){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration");
                    throw std::runtime_error(std::string(" Invalid configuration"));
                }

                const std::string& gpio_name = f_get_gpio_provider_name(iconfig,0);
                
                /*
                * LED parameters: Name, Comment, [GPIO provider, PIN]
                */
                if(iconfig.type==item::ItemTypes::LED){
                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::Led(get_gpio(gpio_name), iconfig.name, iconfig.comment)));
                }
                /*
                * BUTTON parameters: Name, Comment, [GPIO provider, PIN]
                */
                else if(iconfig.type==item::ItemTypes::BUTTON){
                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::Button(
                                get_gpio(gpio_name), 
                                iconfig.name, iconfig.comment, 
                                pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED, 
                                pirobot::gpio::PULL_MODE::PULL_UP)));
                }
                /*
                * DRV8835 parameters: Name, Comment, [GPIO provider, PIN]
                */
                else if(iconfig.type==item::ItemTypes::DRV8835){
                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::Drv8835(get_gpio(gpio_name), iconfig.name, iconfig.comment)));
                }
                else if(iconfig.type==item::ItemTypes::DCMotor){
                /*
                * DCMotor parameters: Name, Comment, [GPIO provider, PIN] [0]=A_PHASE, [1]=A_ENABLE, SUB.ITEM=DRV8835
                */
                    
                    const std::string& pin_a_enable = f_get_gpio_provider_name(iconfig,1);

                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::dcmotor::DCMotor(
                                std::static_pointer_cast<pirobot::item::Drv8835>(get_item(iconfig.sub_item)),
                                get_gpio(gpio_name),
                                get_gpio(pin_a_enable), iconfig.name, iconfig.comment)
                            )
                        );
                }
                else if(iconfig.type==item::ItemTypes::ULN2003Stepper){
                    /*
                    * DCMotor parameters: Name, Comment, [GPIO provider, PIN] 
                        [0]=PHASE_0, 
                        [1]=PHASE_1, 
                        [2]=PHASE_2, 
                        [3]=PHASE_3
                    */
                        
                    const std::string& pin_pb_1 = f_get_gpio_provider_name(iconfig,1);
                    const std::string& pin_pb_2 = f_get_gpio_provider_name(iconfig,2);
                    const std::string& pin_pb_3 = f_get_gpio_provider_name(iconfig,3);

                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(new pirobot::item::ULN2003StepperMotor(
                            get_gpio(gpio_name),
                            get_gpio(pin_pb_1),
                            get_gpio(pin_pb_2),
                            get_gpio(pin_pb_3),
                            iconfig.name, iconfig.comment)));
                
                }
                /*
                * AnlgDgtConvertor parameters: Name, Comment, 
                * [GPIO provider, PIN] GPIO is used for switch ON/OFF (CS/SHDN)
                *
                * TODO: Add support for: 
                    NUmber of Analog Inputs 4/8 (default 8)
                    SPI bus (0/1) (default 0)
                */
                else if(iconfig.type==item::ItemTypes::AnlgDgtConvertor){
                    items_add(iconfig.name, std::shared_ptr<pirobot::item::Item>(new pirobot::mcp320x::MCP320X(
                        std::static_pointer_cast<pirobot::spi::SPI>(get_provider("SPI")),
                        get_gpio(gpio_name),
                        iconfig.name, iconfig.comment)));
                }                        
            }
            break;

        case item::ItemTypes::BLINKER:
            /*
            * BLINKER parameters: Name, SUB.ITEM=LED
            */
        
            if(!f_general_validation(iconfig) || iconfig.sub_item.empty()){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration");
                throw std::runtime_error(std::string(" Invalid configuration"));
            }
    
            items_add(iconfig.name, std::shared_ptr<pirobot::item::Item>(
                new pirobot::item::Blinking<pirobot::item::Led>(
                    std::static_pointer_cast<pirobot::item::Led>(get_item(iconfig.sub_item)),iconfig.name)));
             
            break;
        case item::ItemTypes::AnalogLightMeter:
            /*
            * Analod light meter.
            */
            items_add(iconfig.name, std::shared_ptr<pirobot::item::Item>(
                new pirobot::anlglightmeter::AnalogLightMeter(
                    std::static_pointer_cast<pirobot::analogdata::AnalogDataProviderItf>(
                        std::static_pointer_cast<pirobot::mcp320x::MCP320X>(get_item(iconfig.sub_item))
                    ),
                    iconfig.name, iconfig.comment,
                    iconfig.index, iconfig.value_1, 
                    (iconfig.value_2>0)
                )));
            break;
    }
    logger::log(logger::LLOG::NECECCARY, __func__, " Added Item: " + iconfig.name);
}   
    
/*
 *
 */
bool PiRobot::start(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot is starting..");
    std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;

    for(it = this->items.begin(); it != this->items.end(); ++it){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Initialize " + it->first);

        /*
         * Set callback for Button
         * TODO: Add another types if needed
         */
        if(it->second->have_notify()){
            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Add callback function " + it->first);
            it->second->notify = std::bind(&PiRobot::notify_stm,
                    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }

        const bool res = it->second->initialize();
        if(!res){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Initialization failed " + it->first);
            return false;
            /*TODO: Throw Exception and exit? */
        }
        //(std::dynamic_pointer_cast<item::Led>(it->second))->On();
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + "Robot is started ");
    return true;
}

/*
 *
 */
void PiRobot::stop(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot is stopping..");

    //@TODO: make another solution later
    const std::string debug_data_folder = "/var/log/pi-robot/";

    std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
    for(it = this->items.begin(); it != this->items.end(); ++it){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Stopping " + it->first);
        it->second->stop();

        /*
        * Save debug information
        */
        if(it->second->is_debug()){
            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Unload debug data for " + it->first);
            std::string data_file = debug_data_folder + it->second->name() + ".csv";
            it->second->unload_debug_data("file", data_file);
        }
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot was stopped");
}

void PiRobot::printConfig(){
    std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + 
        " -------------- configuration --------------------------- ");
    for(it = this->items.begin(); it != this->items.end(); ++it){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " " +
            it->first + " " + it->second->printConfig());
    }
}

/*
 * Notify State Machine about Item event
 */
void PiRobot::notify_stm(int itype, std::string& name, void* data){
    if(stm_notification != nullptr){
        mutex_sm.lock();
        stm_notification(itype, name, data);
        mutex_sm.unlock();
    }
    else
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Received notification Item Type: " + std::to_string(itype) + " Name: " + name);
}


} /* namespace pirobot */
