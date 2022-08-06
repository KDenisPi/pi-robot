/*
 * PiRobot_loadconf.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#include "PiRobot.h"
#include "logger.h"

namespace pirobot {
const char TAG[] = "PiRobot";

bool PiRobot::load_gpios(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson){

    const auto json_gpios = cjson->get_array(std::string("gpios"));
    auto json_gpio = cjson->get_first(json_gpios);
    while(json_gpio){

        //
        //Load GPIO attributes (all are mandatory)
        //Note: If name present use it otherwise combine provider+pin as name
        //
        auto gpio_provider = cjson->get_attr_string(json_gpio, "provider");
        auto gpio_pin = cjson->get_attr<int>(json_gpio, "pin");
        auto gpio_mode = cjson->get_attr_string(json_gpio, "mode");
        auto gpio_name = cjson->get_attr_string_def(json_gpio, "name", gpio::Gpio::get_gpio_name(gpio_provider, gpio_pin));
        auto gpio_pull_mode = cjson->get_attr_string_def(json_gpio, "pull_mode", "OFF");
        auto gpio_edge_level = cjson->get_attr_string_def(json_gpio, "edge_level", "NONE");

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

        json_gpio = cjson->get_next(json_gpio);
    }
    return true;
}

}
