/*
 * PiRobot_loadconf.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#include "PiRobot.h"
#include "logger.h"
#include "JsonHelper.h"

#include "Adafruit_PWMServoDriver.h"
#include "GpioProviderFake.h"
#include "GpioProviderSimple.h"
#include "GpioProviderMCP23017.h"
#include "GpioProviderMCP23008.h"
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
#include "Mpu6050.h"
#include "Si7021.h"
#include "sgp30.h"
#include "bmp280.h"
#include "tsl2561.h"
#include "lcd.h"
#include "sled.h"
#include "sledctrl_spi.h"
#include "sledctrl_pwm.h"

namespace pirobot {
const char TAG[] = "PiRobot";

/*
 *
 */
bool PiRobot::configure(const std::string& cfile){
    std::string conf = (cfile.empty() ? get_configuration() : cfile);
    if(conf.empty()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " No Robot configuration");
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Loading Robot hardware configuration ...");
    std::ifstream ijson(conf);
    try{
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Parse JSON from: " + conf);

        jsoncons::json conf = jsoncons::json::parse(ijson);

        auto version = jsonhelper::get_attr<std::string>(conf, "version", "");
        auto real_world = jsonhelper::get_attr<bool>(conf, "real_world", true);
        m_debug_data_folder = jsonhelper::get_attr<std::string>(conf, "debug_data_folder", "/var/log/pi-robot/");

        set_real_world(real_world);
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Version: " + version + " Real World: " + std::to_string(real_world));

        if(is_real_world()) //Under PI
        {
            //Rasbery based GPIO provider present always
            providers["SIMPLE"] = std::make_shared<pirobot::gpio::GpioProviderSimple>();
            providers["I2C"] = std::make_shared<pirobot::i2c::I2C>();
        }
        else{
            providers["SIMPLE"] = std::make_shared<pirobot::gpio::GpioProviderFake>("SIMPLE", 12);
        }

        //Provide function for processing low level events - for GPIO for now
        providers["SIMPLE"]->notify = std::bind(&PiRobot::notify_low, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        //
        // First step - Load providers configuration
        //
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ------------ Load providers ------------");

        if(conf.contains("providers"))
        {
            const jsoncons::json& json_providers  =  conf["providers"];
            for(const auto& provider : json_providers.array_range()){
                std::string provider_type = provider["type"].as<std::string>();
                std::string provider_name = (provider.contains("name") ? provider["name"].as<std::string>() : provider_type);

                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Try to create provider. Name: " + provider_name + " Type: " + provider_type);

                //provider name is unique
                if(is_provider(provider_name)){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Provider with such name is present already. Name: " + provider_name);
                    throw std::runtime_error(std::string("Provider with such name is present already. Name: ") + provider_name);
                }

                if(provider_type == "FAKE"){
                    auto pins = jsonhelper::get_attr<int>(provider, "pins", gpio::GpioProviderFake::s_pins);
                    providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderFake>(provider_name, pins);
                }
                else if(provider_type == "PWM"){ //this provider can be  used with real hardware only
                    if(!is_real_world()){
                        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " PWM provider Adafruit PWM can be used with real hardware only");
                        throw std::runtime_error(std::string("PWM provider Adafruit PWM can be used with real hardware only"));
                    }

                    auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                    auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(provider, "i2c_addr", gpio::Adafruit_PWMServoDriver::s_i2c_addr);
                    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

                    providers[provider_name] = std::make_shared<pirobot::gpio::Adafruit_PWMServoDriver>(provider_name, i2c_provider, i2c_addr);
                }
                else if(provider_type == "MCP23017"){ //this provider can be  used with real hardware only
                    if(!is_real_world()){
                        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider MCP23017 can be used with real hardware only");
                        throw std::runtime_error(std::string("GPIO provider MCP23017 can be used with real hardware only"));
                    }

                    auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                    auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(provider, "i2c_addr", gpio::GpioProviderMCP23017::s_i2c_addr);
                    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

                    providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderMCP23017>(provider_name, i2c_provider, i2c_addr);
                }
                else if(provider_type == "MCP23008"){ //this provider can be  used with real hardware only
                    if(!is_real_world()){
                        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider MCP23008 can be used with real hardware only");
                        throw std::runtime_error(std::string("GPIO provider MCP23008 can be used with real hardware only"));
                    }

                    auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                    auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(provider, "i2c_addr", gpio::GpioProviderMCP23008::s_i2c_addr);
                    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

                    providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderMCP23008>(provider_name, i2c_provider, i2c_addr);
                }
                else if(provider_type == "PCA9685"){ //this provider can be  used with real hardware only
                    if(!is_real_world()){
                        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider PCA9685 can be used with real hardware only");
                        throw std::runtime_error(std::string("GPIO provider PCA9685 can be used with real hardware only"));
                    }

                    auto pwm_name = jsonhelper::get_attr<std::string>(provider, "pwm", "");
                    auto frequency = jsonhelper::get_attr<float>(provider, "frequency", gpio::GpioProviderPCA9685::s_frequency);
                    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Frequency: " + std::to_string(frequency) + " PWM: " + pwm_name);

                    auto pwm_provider = std::static_pointer_cast<pirobot::gpio::Adafruit_PWMServoDriver>(get_provider(pwm_name));
                    providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderPCA9685>(provider_name, pwm_provider, frequency);
                }
                else if(provider_type == "SPI"){ //this provider can be  used with real hardware only

                    pirobot::spi::SPI_config spi_config;

                    spi_config.real_world = is_real_world();
                    spi_config.channels = jsonhelper::get_attr<int>(provider, "channels", 1);

                    if(spi_config.channels == 0 || spi_config.channels > 2){
                        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid number of SPI channels. Should be equal 1 or 2");
                        throw std::runtime_error(std::string("Invalid number of SPI channels"));
                    }

                    spi_config.speed[0] = jsonhelper::get_attr<int>(provider, "speed_1", 1000000); //1MHz
                    spi_config.mode[0] = (spi::SPI_MODE)jsonhelper::get_attr<int>(provider, "mode_1", spi::SPI_MODE::MODE_0);

                    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Channels: " + std::to_string(spi_config.channels) +
                        " Speed[0]: " + std::to_string(spi_config.speed[0]) + " Mode[0]: " + std::to_string(spi_config.mode[0]));

                    if(spi_config.channels == 2){
                        spi_config.speed[1] = jsonhelper::get_attr<int>(provider, "speed_2", 1000000); //1MHz
                        spi_config.mode[1] = (spi::SPI_MODE)jsonhelper::get_attr<int>(provider, "mode_2", spi::SPI_MODE::MODE_0);

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Channels: " + std::to_string(spi_config.channels) +
                        " Speed[1]: " + std::to_string(spi_config.speed[1]) + " Mode[1]: " + std::to_string(spi_config.mode[1]));
                    }

                    add_gpio("SPI_CE0_N", "SIMPLE", gpio::GPIO_MODE::OUT, 10, pirobot::gpio::PULL_MODE::PULL_OFF); //SPI_CE0_N
                    add_gpio("SPI_CE1_N", "SIMPLE", gpio::GPIO_MODE::OUT, 11, pirobot::gpio::PULL_MODE::PULL_OFF); //SPI_CE1_N

                    //Ignore name
                    providers["SPI"] = std::make_shared<pirobot::spi::SPI>("SPI", spi_config, get_gpio("SPI_CE0_N"), get_gpio("SPI_CE1_N"));
                }
                else{
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Unknown provider type");
                    throw std::runtime_error(std::string("Unknown provider type"));
                }
            }
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No provider information");

        /*
        // Create GPIO
        */
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ------------ Load GPIOs ------------");
        if(conf.contains("gpios")){
            const jsoncons::json&  json_gpios  =  conf["gpios"];
            for(const auto& json_gpio : json_gpios.array_range()){

                //
                //Load GPIO attributes (all are mandatory)
                //Note: If name present use it otherwise combine provider+pin as name
                //
                auto gpio_provider = jsonhelper::get_attr_mandatory<std::string>(json_gpio, "provider");
                auto gpio_pin = jsonhelper::get_attr_mandatory<int>(json_gpio, "pin");
                auto gpio_mode = jsonhelper::get_attr_mandatory<std::string>(json_gpio, "mode");
                auto gpio_name = jsonhelper::get_attr<std::string>(json_gpio, "name", gpio::Gpio::get_gpio_name(gpio_provider, gpio_pin));
                auto gpio_pull_mode = jsonhelper::get_attr<std::string>(json_gpio, "pull_mode", "OFF");
                auto gpio_edge_level = jsonhelper::get_attr<std::string>(json_gpio, "edge_level", "NONE");

                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " GPIO Name: " + gpio_name + " Provider: " + gpio_provider +
                    " Pin:" + std::to_string(gpio_pin) + " Mode:" + gpio_mode + " Edge:" + gpio_edge_level);

                if( !(gpio_mode == "IN" || gpio_mode == "OUT" || gpio_mode == "PWM") ){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid GPIO mode value IN/OUT/PWM");
                    throw std::runtime_error(std::string("Invalid GPIO mode value IN/OUT"));
                }

                if( !(gpio_pull_mode == "UP" || gpio_pull_mode == "DOWN" || gpio_pull_mode == "OFF") ){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid GPIO pull mode value UP/DOWM/OFF");
                    throw std::runtime_error(std::string("Invalid GPIO pull mode value UP/DOWM/OFF"));
                }

                if( !(gpio_edge_level == "NONE" || gpio_edge_level == "RAISING" || gpio_edge_level == "FALLING" || gpio_edge_level == "BOTH") ){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid GPIO edge value NONE/RAISING/FALLING/BOTH");
                    throw std::runtime_error(std::string("Invalid GPIO edge value NONE/RAISING/FALLING/BOTH"));
                }

                gpio::GPIO_MODE mode = (gpio_mode == "IN" ? gpio::GPIO_MODE::IN : (gpio_mode == "OUT" ? gpio::GPIO_MODE::OUT : gpio::GPIO_MODE::PWM_OUT));
                gpio::PULL_MODE pull_mode = (gpio_pull_mode == "UP" ? gpio::PULL_MODE::PULL_UP : (gpio_pull_mode == "DOWN" ? gpio::PULL_DOWN : gpio::PULL_MODE::PULL_OFF));

                gpio::GPIO_EDGE_LEVEL edge_level = gpio::GPIO_EDGE_LEVEL::EDGE_NONE;
                if(gpio_edge_level == "RAISING") edge_level = gpio::GPIO_EDGE_LEVEL::EDGE_RAISING;
                else if(gpio_edge_level == "FALLING") edge_level = gpio::GPIO_EDGE_LEVEL::EDGE_FALLING;
                else if(gpio_edge_level == "BOTH") edge_level = gpio::GPIO_EDGE_LEVEL::EDGE_BOTH;


                add_gpio(gpio_name, gpio_provider, mode, gpio_pin, pull_mode, edge_level);
            }
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No GPIO information");

        /*
        // Create Items
        */
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ------------ Load Items ------------");

        auto f_get_gpio_name = [this](const jsoncons::json& object, const std::string& gpio_object_name, const std::string& item_name) -> std::string{
            auto gpio_object = object[gpio_object_name];

            //
            // If GPIO has his own name - use it without checking provider and PIN
            //
            if(gpio_object.contains("name")){
                return jsonhelper::get_attr_mandatory<std::string>(gpio_object, "name");
            }

            auto gpio_provider = jsonhelper::get_attr_mandatory<std::string>(gpio_object, "provider");
            auto gpio_pin = jsonhelper::get_attr_mandatory<int>(gpio_object, "pin");

            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item Name: " + item_name + " GPIO provider: " + gpio_provider + " Pin:" + std::to_string(gpio_pin));

            const auto provider = get_provider(gpio_provider);
            if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
                const std::string msg = " Invalid provider type. " + gpio_provider + " Is not GPIO provider.";
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + msg);
                throw std::runtime_error(msg);
            }

            return gpio::Gpio::get_gpio_name(gpio_provider, gpio_pin);
        };

        auto f_get_motor_direction = [this](const jsoncons::json& object, const std::string& item_name) -> item::MOTOR_DIR{
            std::string direction_name = jsonhelper::get_attr<std::string>(object, "direction", "CLOCKWISE");

            if((direction_name != "CLOCKWISE") && (direction_name != "COUTERCLOCKWISE")){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Motor direction value: " + direction_name + " for " + item_name);
                throw std::runtime_error(std::string(" Invalid Motor direction value: ") + direction_name + " for " + item_name);
            }

            return (direction_name == "CLOCKWISE" ? item::MOTOR_DIR::DIR_CLOCKWISE : item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
        };

        if(conf.contains("items")){
            const jsoncons::json&  json_items  =  conf["items"];
            for(const auto& json_item : json_items.array_range()){

                auto item_type = jsonhelper::get_attr_mandatory<std::string>(json_item, "type");
                auto item_name = jsonhelper::get_attr_mandatory<std::string>(json_item, "name");
                auto item_comment = jsonhelper::get_attr<std::string>(json_item, "comment", "");

                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item Name: " + item_name + " Type: " + item_type + " Comment:" + item_comment);

                item::ItemTypes itype = item::Item::type_by_name(item_type);

                if(itype == item::ItemTypes::UNKNOWN || item_name.empty()){
                    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Item configuration. Type is unknown or name is empty.");
                    throw std::runtime_error(std::string(" Invalid Item configuration. Type is unknown or name is empty."));
                }

                switch(itype){
                    case item::ItemTypes::LED:
                    case item::ItemTypes::BUTTON:
                    case item::ItemTypes::DRV8835:
                    case item::ItemTypes::DCMotor:
                    case item::ItemTypes::AnlgDgtConvertor:
                    {
                        if(!json_item.contains("gpio")){
                            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Item configuration. GPIO information is absent.");
                            throw std::runtime_error(std::string(" Invalid Item configuration. GPIO information is absent."));
                        }

                        std::string gpio_name = f_get_gpio_name(json_item, "gpio", item_name);

                    /*
                    * LED parameters: Name, Comment, [GPIO provider, PIN]
                    */
                        if(itype==item::ItemTypes::LED){
                            items_add(item_name, std::make_shared<pirobot::item::Led>(get_gpio(gpio_name), item_name, item_comment));

                            //Link GPIO with item
                            link_gpio_item(gpio_name, std::make_pair(item_name, itype));
                        }//LED

                    /*
                    * BUTTON parameters: Name, Comment, [GPIO provider, PIN]
                    */
                        else if(itype==item::ItemTypes::BUTTON){
                            auto btn_state = jsonhelper::get_attr<std::string>(json_item, "state", "NOT_PUSHED");
                            auto btn_pull_mode = jsonhelper::get_attr<std::string>(json_item, "pull_mode", "PULL_UP");

                            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Button Name: " + item_name + " State: " + btn_state + " Pull mode:" + btn_pull_mode);

                            if((btn_state != "NOT_PUSHED") && (btn_state != "PUSHED")){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Button state value.");
                                throw std::runtime_error(std::string(" Invalid Button state value."));
                            }

                            if((btn_pull_mode != "PULL_UP") && (btn_pull_mode != "PULL_DOWN")){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Button pull mode value.");
                                throw std::runtime_error(std::string(" Invalid Button pull mode value."));
                            }

                            items_add(item_name,
                                std::make_shared<pirobot::item::Button>(
                                        get_gpio(gpio_name),
                                        item_name, item_comment,
                                        (btn_state == "NOT_PUSHED" ? item::BUTTON_STATE::BTN_NOT_PUSHED : item::BUTTON_STATE::BTN_PUSHED),
                                        (btn_pull_mode == "PULL_UP" ? gpio::PULL_MODE::PULL_UP : gpio::PULL_MODE::PULL_DOWN)));

                            //Link GPIO with item
                            link_gpio_item(gpio_name, std::make_pair(item_name, itype));

                        }//BUTTON

                    /*
                    * DRV8835 parameters: Name, Comment, [GPIO provider, PIN]
                    */
                        else if(itype==item::ItemTypes::DRV8835){
                            auto drv8835_mode = jsonhelper::get_attr<std::string>(json_item, "mode", "PH_EN");

                            if((drv8835_mode != "PH_EN") && (drv8835_mode != "IN_IN")){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Drv8835 mode value.");
                                throw std::runtime_error(std::string(" Invalid Drv8835 mode value."));
                            }

                            items_add(item_name, std::make_shared<pirobot::item::Drv8835>(get_gpio(gpio_name), item_name, item_comment));

                            //Link GPIO with item
                            link_gpio_item(gpio_name, std::make_pair(item_name, itype));
                        }//DRV8835

                    /*
                    * DCMotor parameters: Name, Comment, two GPIOs, DRV8835, Direction
                    */
                        else if(itype==item::ItemTypes::DCMotor){

                            std::string pwm_gpio_name = f_get_gpio_name(json_item, "gpio_pwm", item_name);
                            if(pwm_gpio_name == gpio_name){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid GPIO PWM value (duplicate).");
                                throw std::runtime_error(std::string("Invalid GPIO PWM value (duplicate)."));
                            }

                            auto drv8835_name = jsonhelper::get_attr_mandatory<std::string>(json_item, "drv8835");
                            auto direction = f_get_motor_direction(json_item, item_name);

                            items_add(item_name,
                                std::make_shared<pirobot::item::dcmotor::DCMotor>(
                                        std::static_pointer_cast<pirobot::item::Drv8835>(get_item(drv8835_name)),
                                        get_gpio(gpio_name),
                                        get_gpio(pwm_gpio_name), item_name, item_comment,
                                        direction)
                                );

                            //Link GPIO with item
                            link_gpio_item(gpio_name, std::make_pair(item_name, itype));
                            link_gpio_item(pwm_gpio_name, std::make_pair(item_name, itype));

                        }//DC Motor
                    /*
                    // Analog-Digital converter
                    */
                        else  if(itype== item::ItemTypes::AnlgDgtConvertor){
                            auto analog_inputs =  jsonhelper::get_attr<int>(json_item, "analog_inputs", 8);
                            auto spi_channel  =  jsonhelper::get_attr<int>(json_item, "spi_channel", 0);
                            auto loop_delay  =  jsonhelper::get_attr<unsigned int>(json_item, "delay", 5);

                            items_add(item_name, std::make_shared<pirobot::mcp320x::MCP320X>(
                                std::static_pointer_cast<pirobot::spi::SPI>(get_provider("SPI")),
                                get_gpio(gpio_name),
                                item_name,
                                item_comment,
                                (analog_inputs == 8 ? mcp320x::MCP320X_INPUTS::MCP3208 : mcp320x::MCP320X_INPUTS::MCP3204),
                                (spi_channel == 0 ? spi::SPI_CHANNELS::SPI_0 : spi::SPI_CHANNELS::SPI_1),
                                loop_delay));

                            //Link GPIO with item
                            link_gpio_item(gpio_name, std::make_pair(item_name, itype));
                        }
                    }//Single GPIO based Items
                    break;

                    case item::ItemTypes::ULN2003Stepper:
                    {
                            /*
                            DCMotor parameters: Name, Comment, [GPIO provider, PIN]
                            [0]=PHASE_0, [1]=PHASE_1, [2]=PHASE_2, [3]=PHASE_3
                            */
                            std::string gpio_phase_0_name = f_get_gpio_name(json_item, "gpio_phase_0", item_name);
                            std::string gpio_phase_1_name = f_get_gpio_name(json_item, "gpio_phase_1", item_name);
                            std::string gpio_phase_2_name = f_get_gpio_name(json_item, "gpio_phase_2", item_name);
                            std::string gpio_phase_3_name = f_get_gpio_name(json_item, "gpio_phase_3", item_name);

                            auto direction = f_get_motor_direction(json_item, item_name);

                            items_add(item_name, std::make_shared<pirobot::item::ULN2003StepperMotor>(
                                    get_gpio(gpio_phase_0_name),
                                    get_gpio(gpio_phase_1_name),
                                    get_gpio(gpio_phase_2_name),
                                    get_gpio(gpio_phase_3_name),
                                    item_name, item_comment, direction));

                            //Link GPIO with item
                            link_gpio_item(gpio_phase_0_name, std::make_pair(item_name, itype));
                            link_gpio_item(gpio_phase_1_name, std::make_pair(item_name, itype));
                            link_gpio_item(gpio_phase_2_name, std::make_pair(item_name, itype));
                            link_gpio_item(gpio_phase_3_name, std::make_pair(item_name, itype));
                    }
                    break;

                    case item::ItemTypes::LCD:
                    {
                            /*

                            */
                            std::string gpio_rs_name = f_get_gpio_name(json_item, "gpio_rs", item_name);
                            std::string gpio_enable_name = f_get_gpio_name(json_item, "gpio_enable", item_name);

                            std::string gpio_data_4_name = f_get_gpio_name(json_item, "gpio_data_4", item_name);
                            std::string gpio_data_5_name = f_get_gpio_name(json_item, "gpio_data_5", item_name);
                            std::string gpio_data_6_name = f_get_gpio_name(json_item, "gpio_data_6", item_name);
                            std::string gpio_data_7_name = f_get_gpio_name(json_item, "gpio_data_7", item_name);

                            std::string gpio_backlite_name = f_get_gpio_name(json_item, "gpio_backlite", item_name);

                            auto lines  =  jsonhelper::get_attr<int>(json_item, "lines", 1);
                            auto bitmode  =  jsonhelper::get_attr<int>(json_item, "bitmode", 4);
                            auto dots  =  jsonhelper::get_attr<int>(json_item, "dots", 8);

                            if(bitmode == 4){
                                items_add(item_name, std::make_shared<pirobot::item::lcd::Lcd>(
                                        get_gpio(gpio_rs_name),
                                        get_gpio(gpio_enable_name),
                                        get_gpio(gpio_data_4_name),
                                        get_gpio(gpio_data_5_name),
                                        get_gpio(gpio_data_6_name),
                                        get_gpio(gpio_data_7_name),
                                        get_gpio(gpio_backlite_name),
                                        item_name, item_comment,
                                        lines, bitmode, dots));

                                //Link GPIO with item
                                link_gpio_item(gpio_rs_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_enable_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_data_4_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_data_5_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_data_6_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_data_7_name, std::make_pair(item_name, itype));
                                link_gpio_item(gpio_backlite_name, std::make_pair(item_name, itype));
                            }
                    }
                    break;

                    case item::ItemTypes::AnalogLightMeter:
                /*
                * Analod light meter.
                */
                    {
                        auto ad_convertor = jsonhelper::get_attr_mandatory<std::string>(json_item, "ad_convertor");
                        auto analog_input_index = jsonhelper::get_attr_mandatory<int>(json_item, "analog_input_index");

                        auto debug_mode  =  jsonhelper::get_attr<bool>(json_item, "debug", false);
                        auto debug_buffer_size  =  jsonhelper::get_attr<int>(json_item, "debug_buffer_size", 2048);
                        auto value_diff_for_event  =  jsonhelper::get_attr<int>(json_item, "value_diff_for_event", 0);

                        items_add(item_name, std::make_shared<pirobot::anlglightmeter::AnalogLightMeter>(
                                std::static_pointer_cast<pirobot::analogdata::AnalogDataProviderItf>(
                                    std::static_pointer_cast<pirobot::mcp320x::MCP320X>(get_item(ad_convertor))
                                ),
                                item_name,
                                item_comment,
                                analog_input_index,
                                value_diff_for_event,
                                debug_mode,
                                debug_buffer_size
                            ));
                    }
                    break;

                    case item::ItemTypes::BLINKER:
                /*
                * BLINKER parameters: Name, SUB.ITEM=LED
                */
                    {
                        auto led = jsonhelper::get_attr_mandatory<std::string>(json_item, "led");
                        auto tm_on  =  jsonhelper::get_attr<int>(json_item, "tm_on", 250);
                        auto tm_off  =  jsonhelper::get_attr<int>(json_item, "tm_off", 500);
                        auto blinks  =  jsonhelper::get_attr<int>(json_item, "blinks", 0);

                        items_add(item_name, std::make_shared<pirobot::item::Blinking<pirobot::item::Led>>(
                                std::static_pointer_cast<pirobot::item::Led>(get_item(led)),
                                item_name,
                                item_comment,
                                tm_on,
                                tm_off,
                                blinks));
                    }
                    break;
                    /*
                    * MPU-6050 Accelerometer + Gyro
                    */
                    case item::ItemTypes::MPU6050:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                        auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(json_item, "i2c_addr", MPU6050_I2C_ADDRESS);
                        auto loop_delay  =  jsonhelper::get_attr<unsigned int>(json_item, "delay", 100);

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " I2C address: " + std::to_string(i2c_addr));
                        items_add(item_name, std::make_shared<pirobot::mpu6050::Mpu6050>(item_name, i2c_provider, item_comment, i2c_addr, loop_delay));
                    }
                    break;

                    /*
                    *  Si7021 I2C Humidity and Temperature Sensor
                    */
                    case item::ItemTypes::SI7021:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
                        items_add(item_name, std::make_shared<pirobot::item::Si7021>(item_name, i2c_provider, item_comment));
                    }
                    break;

                    /*
                    *  SGP30 I2C Sensurion Gas Platform
                    */
                    case item::ItemTypes::SGP30:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
                        items_add(item_name, std::make_shared<pirobot::item::Sgp30>(item_name, i2c_provider, item_comment));
                    }
                    break;

                    /*
                    * BMP280 I2C Digital Presure Sensor
                    */
                    case item::ItemTypes::BMP280:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);

                        auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(json_item, "i2c_addr", pirobot::item::Bmp280::s_i2c_addr); //0x76
                        auto smode = jsonhelper::get_attr<std::string>(json_item, "mode", "FORCED");
                        auto pressure_oversampling = (uint8_t)jsonhelper::get_attr<int>(json_item, "pressure_oversampling", 1);
                        auto temperature_oversampling = (uint8_t)jsonhelper::get_attr<int>(json_item, "temperature_oversampling", 0xFF);
                        auto standby_time = (uint8_t)jsonhelper::get_attr<int>(json_item, "standby_time", 5);
                        auto filter = (uint8_t)jsonhelper::get_attr<int>(json_item, "filter", 0);
                        auto spi = jsonhelper::get_attr<int>(json_item, "SPI", 0);
                        auto spi_channel  =  jsonhelper::get_attr<int>(json_item, "spi_channel", 0);

                        auto mode = pirobot::item::Bmp280::get_mode_by_name(smode);

                        if( (filter != 0) && (filter != 2) && (filter != 4) &&(filter != 8) && (filter != 16) ){
                            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid filter value. Possible values 0,2,4,8,16");
                            throw std::runtime_error(std::string("Invalid filter value. Possible values 0,2,4,8,16"));
                        }

                        items_add(item_name, std::make_shared<pirobot::item::Bmp280>(
                                    item_name, i2c_provider, i2c_addr, mode, pressure_oversampling, temperature_oversampling, standby_time, filter, spi, spi_channel, item_comment));
                    }
                    break;

                    /*
                    *  TSL2561  I2C Light-to-Digital Converter
                    */
                    case item::ItemTypes::TSL2561:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                        auto i2c_addr = (uint8_t)jsonhelper::get_attr<int>(json_item, "i2c_addr", pirobot::item::Tsl2561::s_i2c_addr); //0x39

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
                        items_add(item_name, std::make_shared<pirobot::item::Tsl2561>(item_name, i2c_provider, i2c_addr, item_comment));
                    }
                    break;

                    /*
                    *  SPI based LED stripe controller
                    */
                    case item::ItemTypes::SLEDCRTLSPI:
                    {
                        auto spi_channel  =  jsonhelper::get_attr<int>(json_item, "spi_channel", 0);
                        auto stripes  =  jsonhelper::get_attr<int>(json_item, "stripes", 0);

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
                        items_add(item_name, std::make_shared<pirobot::item::sledctrl::SLedCtrlSpi>(
                                        std::static_pointer_cast<pirobot::spi::SPI>(get_provider("SPI")),
                                        stripes,
                                        item_name,
                                        item_comment,
                                        (spi_channel == 0 ? spi::SPI_CHANNELS::SPI_0 : spi::SPI_CHANNELS::SPI_1)
                                    ));

                        auto sledctrl = std::static_pointer_cast<pirobot::item::sledctrl::SLedCtrlSpi>(get_item(item_name));

                        auto json_sled  =  json_item["stripe"];
                        for(const auto& stripe : json_sled.array_range()){
                            auto leds  =  jsonhelper::get_attr_mandatory<int>(stripe, "leds");
                            auto sled_name  =  jsonhelper::get_attr_mandatory<std::string>(stripe, "name");
                            auto sled_comm  =  jsonhelper::get_attr<std::string>(stripe, "comment", "");
                            auto sled_type  =  jsonhelper::get_attr_mandatory<std::string>(stripe, "type");
                            if( sled_type != "WS2810" ){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid LED stripe type " + sled_type);
                                throw std::runtime_error(std::string(" Invalid LED stripe type" + sled_type));
                            }
                            pirobot::item::SLedType stype = (sled_type == "WS2810" ? pirobot::item::SLedType::WS2810 : pirobot::item::SLedType::WS2812B);

                            sledctrl->add_sled(std::make_shared<pirobot::item::SLed>(leds, stype, sled_name, sled_comm));
                        }
                    }
                    break;

                    /*
                    *  PWM based LED stripe controller
                    */
                    case item::ItemTypes::SLEDCRTLPWM:
                    {
                        auto stripes  =  jsonhelper::get_attr<int>(json_item, "stripes", 0);
                        std::string pwm_gpio = f_get_gpio_name(json_item, "gpio_pwm", item_name);

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);

                        items_add(item_name, std::make_shared<pirobot::item::sledctrl::SLedCtrlPwm>(
                                        stripes,
                                        item_name,
                                        get_gpio(pwm_gpio),
                                        item_comment
                                    ));

                        //Link GPIO with item
                        link_gpio_item(pwm_gpio, std::make_pair(item_name, itype));

                        auto sledctrl = std::static_pointer_cast<pirobot::item::sledctrl::SLedCtrlSpi>(get_item(item_name));

                        auto json_sled  =  json_item["stripe"];
                        for(const auto& stripe : json_sled.array_range()){
                            auto leds  =  jsonhelper::get_attr_mandatory<int>(stripe, "leds");
                            auto sled_name  =  jsonhelper::get_attr_mandatory<std::string>(stripe, "name");
                            auto sled_comm  =  jsonhelper::get_attr<std::string>(stripe, "comment", "");
                            auto sled_type  =  jsonhelper::get_attr_mandatory<std::string>(stripe, "type");
                            if( sled_type != "WS2812B" ){
                                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid LED stripe type " + sled_type);
                                throw std::runtime_error(std::string(" Invalid LED stripe type" + sled_type));
                            }
                            pirobot::item::SLedType stype = (sled_type == "WS2810" ? pirobot::item::SLedType::WS2810 : pirobot::item::SLedType::WS2812B);

                            sledctrl->add_sled(std::make_shared<pirobot::item::SLed>(leds, stype, sled_name, sled_comm));
                        }
                    }

                }//Item types
            }//Items collection loading
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No Items information");
    }
    catch(jsoncons::ser_error& perr){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration " +
            perr.what() + " Line: " + std::to_string(perr.line()) + " Column: " + std::to_string(perr.column()));
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot configuration is finished");

    //print loaded configuration
    printConfig();

    return true;
}

/*
    Low level notification function (provider callback etc)
    Parameters:
        ptype - provider type
        pname - provider name
        pdata - provider data used for notification
*/
void PiRobot::notify_low(const pirobot::provider::PROVIDER_TYPE ptype, const std::string& pname, std::shared_ptr<pirobot::provider::ProviderData> pdata) {
    if(ptype == pirobot::provider::PROVIDER_TYPE::PROV_GPIO){
        const std::string gpio_name = gpio::Gpio::get_gpio_name(pname, pdata->pin());
        auto gpio_item = gpio_items.find(gpio_name);
        if(gpio_item == gpio_items.end()){
            //It is strange
            return;
        }
        
        //check all Items used this GPIO
        const std::shared_ptr<std::vector<item_name_type>> v_items = gpio_item->second;
        for(auto item_info : *(v_items)){
            /*
            * If this GPIO used by buttom notify system about state change
            */
            if(item_info.second == item::ItemTypes::BUTTON){
                const std::shared_ptr<pirobot::item::Button> btn = std::static_pointer_cast<pirobot::item::Button>(get_item(item_info.first));
                btn->process_level(gpio::Gpio::value_to_level(pdata->value()));
            }
        }
    }

}

}//namespace
