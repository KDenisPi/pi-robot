/*
 * PiRobot_loadconf.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#include "PiRobot.h"
#include "logger.h"

#include "sledctrl_spi.h"
#include "sledctrl_pwm.h"

namespace pirobot {
const char TAG[] = "PiRobot";

bool PiRobot::load_sled(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson, const piutils::cjson_wrap::cj_obj json_item,
            item::ItemTypes itype, const std::string& item_name, const std::string& item_comment){
    if(item::ItemTypes::SLEDCRTLSPI == itype)
    {
        auto spi_channel  =  cjson->get_attr_def<int>(json_item, "spi_channel", 0);
        auto stripes  =  cjson->get_attr_def<int>(json_item, "stripes", 0);

        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
        items_add(item_name, std::make_shared<pirobot::item::sledctrl::SLedCtrlSpi>(
                        std::static_pointer_cast<pirobot::spi::SPI>(get_provider("SPI")),
                        stripes,
                        item_name,
                        item_comment,
                        (spi_channel == 0 ? spi::SPI_CHANNELS::SPI_0 : spi::SPI_CHANNELS::SPI_1)
                    ));

        auto sledctrl = std::static_pointer_cast<pirobot::item::sledctrl::SLedCtrlSpi>(get_item(item_name));

        const auto json_sled = cjson->get_array(json_item, std::string("stripe"));
        auto stripe = cjson->get_first(json_sled);
        while(stripe){
            auto leds  =  cjson->get_attr<int>(stripe, "leds");
            auto sled_name  =  cjson->get_attr_string(stripe, "name");
            auto sled_comm  =  cjson->get_attr_string_def(stripe, "comment", "");
            auto sled_type  =  cjson->get_attr_string(stripe, "type");
            if( sled_type != "WS2810" ){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid LED stripe type " + sled_type);
                throw std::runtime_error(std::string(" Invalid LED stripe type" + sled_type));
            }
            pirobot::item::SLedType stype = (sled_type == "WS2810" ? pirobot::item::SLedType::WS2810 : pirobot::item::SLedType::WS2812B);

            sledctrl->add_sled(std::make_shared<pirobot::item::SLed>(leds, stype, sled_name, sled_comm));
            stripe = cjson->get_next(stripe);
        }
    }
    /*
    *  PWM based LED stripe controller
    */
    else if(item::ItemTypes::SLEDCRTLPWM == itype)
    {
        auto stripes  =  cjson->get_attr_def<int>(json_item, "stripes", 0);
        std::string pwm_gpio = f_get_gpio_name(cjson, json_item, "gpio_pwm", item_name);

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

        const auto json_sled = cjson->get_array(json_item, std::string("stripe"));
        auto stripe = cjson->get_first(json_sled);
        while(stripe){
            auto leds  =  cjson->get_attr<int>(stripe, "leds");
            auto sled_name  =  cjson->get_attr_string(stripe, "name");
            auto sled_comm  =  cjson->get_attr_string_def(stripe, "comment", "");
            auto sled_type  =  cjson->get_attr_string(stripe, "type");
            if( sled_type != "WS2812B" ){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid LED stripe type " + sled_type);
                throw std::runtime_error(std::string(" Invalid LED stripe type" + sled_type));
            }
            pirobot::item::SLedType stype = (sled_type == "WS2810" ? pirobot::item::SLedType::WS2810 : pirobot::item::SLedType::WS2812B);

            sledctrl->add_sled(std::make_shared<pirobot::item::SLed>(leds, stype, sled_name, sled_comm));
            stripe = cjson->get_next(stripe);
        }
    }

}

}