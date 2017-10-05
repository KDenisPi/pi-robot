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

    if(is_real_world()) //Under PI
    {
        int wiPi = wiringPiSetup();
        //Rasbery based GPIO provider present always
        providers["SIMPLE"] = std::shared_ptr<pirobot::gpio::GpioProvider>(new pirobot::gpio::GpioProviderSimple());
    }
    else{
        providers["SIMPLE"] = std::shared_ptr<pirobot::gpio::GpioProvider>(new pirobot::gpio::GpioProviderFake());
    }
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
std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const int id) const{
    auto pgpio = this->gpios.find(id);
    if(pgpio == gpios.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Absent requested GPIO with ID " + std::to_string(id));
        throw std::runtime_error(std::string("No GPIO with ID: ") + std::to_string(id));
    }
    return pgpio->second;
}

/*
 * Get GPIO by provider name and PIN number
 */
 std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const std::string provider_name, const int pin)  const{
    auto provider = get_provider(provider_name);
    auto gpio_provider = std::static_pointer_cast<pirobot::gpio::GpioProvider>(provider);
    return get_gpio(gpio_provider->getStartPin() + pin);
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
* Add provider
*/
void PiRobot::add_provider(const std::string& type, const std::string& name){

    auto provider = this->providers.find(name);
    //provider name is unique
    if(provider != providers.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Provider with such name is present already. Name: " + name);
        throw std::runtime_error(std::string("Provider with such name is present already. Name: ") + name);
    }

    if(type == "FAKE"){
        providers[name] = std::shared_ptr<pirobot::provider::Provider>(new pirobot::gpio::GpioProviderFake(name, 10, 16));
    }
    else if(type == "MCP23017"){ //this provider can be  used with real hardware only
        if(!is_real_world()){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider MCP23017 can be used with real hardware only");
            throw std::runtime_error(std::string("GPIO provider MCP23017 can be used with real hardware only"));
        }
        providers[name] = std::shared_ptr<pirobot::provider::Provider>(new pirobot::gpio::GpioProviderMCP23017(name));
    }
    else if(type == "PCA9685"){ //this provider can be  used with real hardware only
        if(!is_real_world()){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider PCA9685 can be used with real hardware only");
            throw std::runtime_error(std::string("GPIO provider PCA9685 can be used with real hardware only"));
        }
        providers[name] = std::shared_ptr<pirobot::provider::Provider>(
            new pirobot::gpio::GpioProviderPCA9685(name, 
                std::shared_ptr<pirobot::gpio::Adafruit_PWMServoDriver>(new pirobot::gpio::Adafruit_PWMServoDriver())
            ));
    }
    else if(type == "SPI"){ //this provider can be  used with real hardware only
        pirobot::spi::SPI_config spi_config;
        spi_config.real_world = is_real_world();

        add_gpio("SIMPLE", pirobot::gpio::GPIO_MODE::OUT, 10); //SPI_CE0_N
        add_gpio("SIMPLE", pirobot::gpio::GPIO_MODE::OUT, 11); //SPI_CE1_N
        //Ignore name
        providers["SPI"] = std::shared_ptr<pirobot::provider::Provider>(
            new pirobot::spi::SPI("SPI", spi_config, get_gpio("SIMPLE", 10), get_gpio("SIMPLE", 11)));
    }
    else{
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Unknown provider type");
        throw std::runtime_error(std::string("Unknown provider type"));
    }
}

/*
* Create GPIO for the provider. Provider should created before using add_provider function
*/
void PiRobot::add_gpio(const std::string& provider_name, 
    const pirobot::gpio::GPIO_MODE gpio_mode, const int pin) noexcept(false){
    const auto provider = get_provider(provider_name);
    if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid provider type");
        throw std::runtime_error(std::string("Invalid provider type"));
    }
    auto gpio_provider = std::static_pointer_cast<pirobot::gpio::GpioProvider>(provider);
    int global_pin = gpio_provider->getStartPin() + pin;
    // Validate pin number
    if(!gpio_provider->is_valid_pin(global_pin)){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid pin number: " + std::to_string(pin));
        throw std::runtime_error(std::string(" Invalid pin number: ") + std::to_string(pin));
    }
    auto pgpio = this->gpios.find(global_pin);
    if(pgpio != gpios.end()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO with such PIN is present already PIN: " + std::to_string(pin));
        return;
    }
        
    gpios_add(global_pin,
            std::shared_ptr<pirobot::gpio::Gpio>(
                new pirobot::gpio::Gpio(global_pin,	gpio_mode, gpio_provider)
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
/*        
        int asize = iconfig.gpios.size();    
        for(auto gpio : iconfig.gpios){
            if(gpio.first.empty() || gpio.second < 0)
                return false;
        }    
*/
        return true;
    };

    auto f_general_validation = [](const pirobot::item::ItemConfig& iconfig)->bool {
        if(iconfig.type == item::ItemTypes::UNKNOWN || iconfig.name.empty())
            return false;

        return true;    
    };

    auto f_get_gpio_provider_abs_pin = [this](const pirobot::item::ItemConfig& iconfig, int idx) -> int{
        const auto gpio = iconfig.gpios[idx];
        const auto provider = get_provider(gpio.first);
        if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid provider type");
            throw std::runtime_error(std::string("Invalid provider type"));
        }

        auto gpio_provider = std::static_pointer_cast<pirobot::gpio::GpioProvider>(provider);
        return gpio_provider->getStartPin() + gpio.second;
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

                const int pin = f_get_gpio_provider_abs_pin(iconfig,0);
                
                /*
                * LED parameters: Name, Comment, [GPIO provider, PIN]
                */
                if(iconfig.type==item::ItemTypes::LED){
                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::Led(get_gpio(pin), iconfig.name, iconfig.comment)));
                }
                /*
                * BUTTON parameters: Name, Comment, [GPIO provider, PIN]
                */
                else if(iconfig.type==item::ItemTypes::BUTTON){
                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::Button(
                                get_gpio(pin), 
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
                            new pirobot::item::Drv8835(get_gpio(pin), iconfig.name, iconfig.comment)));
                }
                else if(iconfig.type==item::ItemTypes::DCMotor){
                /*
                * DCMotor parameters: Name, Comment, [GPIO provider, PIN] [0]=A_PHASE, [1]=A_ENABLE, SUB.ITEM=DRV8835
                */
                    
                    const int pin_a_enable = f_get_gpio_provider_abs_pin(iconfig,1);

                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(
                            new pirobot::item::dcmotor::DCMotor(
                                std::static_pointer_cast<pirobot::item::Drv8835>(get_item(iconfig.sub_item)),
                                get_gpio(pin),
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
                        
                    const int pin_pb_1 = f_get_gpio_provider_abs_pin(iconfig,1);
                    const int pin_pb_2 = f_get_gpio_provider_abs_pin(iconfig,2);
                    const int pin_pb_3 = f_get_gpio_provider_abs_pin(iconfig,3);

                    items_add(iconfig.name, 
                        std::shared_ptr<pirobot::item::Item>(new pirobot::item::ULN2003StepperMotor(
                            get_gpio(pin),
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
                        get_gpio(pin),
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
        case item::ItemTypes::AnalogMeter:
            /*
            * Analod light meter.
            */
            items_add(iconfig.name, std::shared_ptr<pirobot::item::Item>(
                new pirobot::anlglightmeter::AnalogLightMeter(
                    std::static_pointer_cast<pirobot::analogdata::AnalogDataProviderItf>(
                        std::static_pointer_cast<pirobot::mcp320x::MCP320X>(get_item(iconfig.sub_item))
                    ),
                    iconfig.name, iconfig.comment,
                    iconfig.index
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

    std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
    for(it = this->items.begin(); it != this->items.end(); ++it){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Stopping " + it->first);
        it->second->stop();
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot was stopped");
}

/*
 *
 */
bool PiRobot::configure(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot configuration is starting..");

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot configuration is finished");
    return true;
}

void PiRobot::printConfig(){
    std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + 
        "-------------- configuration ---------------------------");
    for(it = this->items.begin(); it != this->items.end(); ++it){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + 
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
