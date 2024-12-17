/*
 * PiRobot_loadconf.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#include "PiRobot.h"
#include "logger.h"

#include "MCP320X.h"
#include "led.h"
#include "button.h"
#include "Drv8835.h"
#include "DCMotor.h"
#include "blinking.h"

namespace pirobot {
const char TAG[] = "PiRobot";

bool PiRobot::load_item_one_gpio(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson, const piutils::cjson_wrap::cj_obj json_item,
            item::ItemTypes itype, const std::string& item_name, const std::string& item_comment){

    if(itype== item::ItemTypes::BLINKER){
        auto led = cjson->get_attr_string(json_item, "led");
        auto tm_on  =  cjson->get_attr_def<int>(json_item, "tm_on", 250);
        auto tm_off  =  cjson->get_attr_def<int>(json_item, "tm_off", 500);
        auto blinks  =  cjson->get_attr_def<int>(json_item, "blinks", 0);

        items_add(item_name, std::make_shared<pirobot::item::Blinking<pirobot::item::Led>>(
                get_item<pirobot::item::Led>(led),
                item_name,
                item_comment,
                tm_on,
                tm_off,
                blinks));

        return true;
    }


    std::string gpio_name = f_get_gpio_name(cjson, json_item, "gpio", item_name);

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
        auto btn_state = cjson->get_attr_string_def(json_item, "state", "NOT_PUSHED");
        auto btn_pull_mode = cjson->get_attr_string_def(json_item, "pull_mode", "PULL_UP");

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
        auto drv8835_mode = cjson->get_attr_string_def(json_item, "mode", "PH_EN");

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

        std::string pwm_gpio_name = f_get_gpio_name(cjson, json_item, "gpio_pwm", item_name);
        if(pwm_gpio_name == gpio_name){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid GPIO PWM value (duplicate).");
            throw std::runtime_error(std::string("Invalid GPIO PWM value (duplicate)."));
        }

        auto drv8835_name = cjson->get_attr_string(json_item, "drv8835");
        auto direction = f_get_motor_direction(cjson, json_item, item_name);

        items_add(item_name,
            std::make_shared<pirobot::item::dcmotor::DCMotor>(
                    get_item<pirobot::item::Drv8835>(drv8835_name),
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
        auto analog_inputs =  cjson->get_attr_def<int>(json_item, "analog_inputs", 8);
        auto spi_channel  =  cjson->get_attr_def<int>(json_item, "spi_channel", 0);
        auto loop_delay  =  cjson->get_attr_def<unsigned int>(json_item, "delay", 5);
        auto dev_type = cjson->get_attr_string_def(json_item, "dtype", "MCP3208");

        pirobot::mcp320x::MCP3XXX_DEVICE_TYPE anlg_device = mcp320x::MCP3XXX_DEVICE_TYPE::MCP3208;
        if(analog_inputs == 8){
            anlg_device = (dev_type=="MCP3008" ? mcp320x::MCP3XXX_DEVICE_TYPE::MCP3008 : mcp320x::MCP3XXX_DEVICE_TYPE::MCP3208);
        }
        else if(analog_inputs == 4){
            anlg_device = (dev_type=="MCP3004" ? mcp320x::MCP3XXX_DEVICE_TYPE::MCP3004 : mcp320x::MCP3XXX_DEVICE_TYPE::MCP3204);
        }

        items_add(item_name, std::make_shared<pirobot::mcp320x::MCP320X>(
            std::static_pointer_cast<pirobot::spi::SPI>(get_provider("SPI")),
            get_gpio(gpio_name),
            item_name,
            item_comment,
            anlg_device,
            (spi_channel == 0 ? spi::SPI_CHANNELS::SPI_0 : spi::SPI_CHANNELS::SPI_1),
            loop_delay));

        //Link GPIO with item
        link_gpio_item(gpio_name, std::make_pair(item_name, itype));
    }

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
                auto btn = get_item<pirobot::item::Button>(item_info.first);
                btn->process_level(gpio::Gpio::value_to_level(pdata->value()));
            }
        }
    }
}


}