/*
 * PiRobot_loadconf.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#include "PiRobot.h"
#include "logger.h"

#include "GpioProviderFake.h"
#include "GpioProviderSimple.h"
#include "GpioProviderMCP23017.h"
#include "GpioProviderMCP23008.h"
#include "GpioProviderPCA9685.h"
#include "SPI.h"
#include "I2C.h"

namespace pirobot {
const char TAG[] = "PiRobot";

bool PiRobot::load_providers(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson){
    const auto json_providers = cjson->get_array(std::string("providers"));
    auto provider = cjson->get_first(json_providers);
    while(provider){

        std::string provider_type = cjson->get_attr_string(provider, "type");
        std::string provider_name = (cjson->contains("name", provider) ? cjson->get_attr_string(provider, "name") : provider_type);

        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Try to create provider. Name: " + provider_name + " Type: " + provider_type);

        //provider name is unique
        if(is_provider(provider_name)){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Provider with such name is present already. Name: " + provider_name);
            throw std::runtime_error(std::string("Provider with such name is present already. Name: ") + provider_name);
        }

        if(provider_type == "FAKE"){
            auto pins = cjson->get_attr_def<int>(provider, "pins", gpio::GpioProviderFake::s_pins);
            providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderFake>(provider_name, pins);
        }
        else if(provider_type == "PWM"){ //this provider can be  used with real hardware only
            if(!is_real_world()){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " PWM provider Adafruit PWM can be used with real hardware only");
                throw std::runtime_error(std::string("PWM provider Adafruit PWM can be used with real hardware only"));
            }

            auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
            auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(provider, "i2c_addr", gpio::Adafruit_PWMServoDriver::s_i2c_addr);
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

            providers[provider_name] = std::make_shared<pirobot::gpio::Adafruit_PWMServoDriver>(provider_name, i2c_provider, i2c_addr);
        }
        else if(provider_type == "MCP23017"){ //this provider can be  used with real hardware only
            if(!is_real_world()){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider MCP23017 can be used with real hardware only");
                throw std::runtime_error(std::string("GPIO provider MCP23017 can be used with real hardware only"));
            }

            auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
            auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(provider, "i2c_addr", gpio::GpioProviderMCP23017::s_i2c_addr);
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

            providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderMCP23017>(provider_name, i2c_provider, i2c_addr);
        }
        else if(provider_type == "MCP23008"){ //this provider can be  used with real hardware only
            if(!is_real_world()){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider MCP23008 can be used with real hardware only");
                throw std::runtime_error(std::string("GPIO provider MCP23008 can be used with real hardware only"));
            }

            auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
            auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(provider, "i2c_addr", gpio::GpioProviderMCP23008::s_i2c_addr);
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " I2C address: " + std::to_string(i2c_addr));

            providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderMCP23008>(provider_name, i2c_provider, i2c_addr);
        }
        else if(provider_type == "PCA9685"){ //this provider can be  used with real hardware only
            if(!is_real_world()){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " GPIO provider PCA9685 can be used with real hardware only");
                throw std::runtime_error(std::string("GPIO provider PCA9685 can be used with real hardware only"));
            }

            auto pwm_name = cjson->get_attr_string_def(provider, "pwm", "");
            auto frequency = cjson->get_attr_def<float>(provider, "frequency", gpio::GpioProviderPCA9685::s_frequency);
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Frequency: " + std::to_string(frequency) + " PWM: " + pwm_name);

            auto pwm_provider = std::static_pointer_cast<pirobot::gpio::Adafruit_PWMServoDriver>(get_provider(pwm_name));
            providers[provider_name] = std::make_shared<pirobot::gpio::GpioProviderPCA9685>(provider_name, pwm_provider, frequency);
        }
        else if(provider_type == "SPI"){ //this provider can be  used with real hardware only

            pirobot::spi::SPI_config spi_config;

            spi_config.real_world = is_real_world();
            spi_config.channels = cjson->get_attr_def<int>(provider, "channels", 1);

            if(spi_config.channels == 0 || spi_config.channels > 2){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid number of SPI channels. Should be equal 1 or 2");
                throw std::runtime_error(std::string("Invalid number of SPI channels"));
            }

            spi_config.speed[0] = cjson->get_attr_def<int>(provider, "speed_1", 1000000); //1MHz
            spi_config.mode[0] = (spi::SPI_MODE)cjson->get_attr_def<int>(provider, "mode_1", spi::SPI_MODE::MODE_0);

            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Channels: " + std::to_string(spi_config.channels) +
                " Speed[0]: " + std::to_string(spi_config.speed[0]) + " Mode[0]: " + std::to_string(spi_config.mode[0]));

            if(spi_config.channels == 2){
                spi_config.speed[1] = cjson->get_attr_def<int>(provider, "speed_2", 1000000); //1MHz
                spi_config.mode[1] = (spi::SPI_MODE)cjson->get_attr_def<int>(provider, "mode_2", spi::SPI_MODE::MODE_0);

                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Provider: " + provider_name + " Channels: " + std::to_string(spi_config.channels) +
                " Speed[1]: " + std::to_string(spi_config.speed[1]) + " Mode[1]: " + std::to_string(spi_config.mode[1]));
            }

            add_gpio("SPI_CE0_N", "SIMPLE", gpio::GPIO_MODE::OUT, 19, pirobot::gpio::PULL_MODE::PULL_OFF); //SPI_CE0_N
            add_gpio("SPI_CE1_N", "SIMPLE", gpio::GPIO_MODE::OUT, 18, pirobot::gpio::PULL_MODE::PULL_OFF); //SPI_CE1_N

            //Ignore name
            providers["SPI"] = std::make_shared<pirobot::spi::SPI>("SPI", spi_config, get_gpio("SPI_CE0_N"), get_gpio("SPI_CE1_N"));
        }
        else{
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Unknown provider type");
            throw std::runtime_error(std::string("Unknown provider type"));
        }

        provider = cjson->get_next(provider);
    }
    return true;
}

}