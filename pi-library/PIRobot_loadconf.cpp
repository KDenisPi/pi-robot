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

#include "Adafruit_PWMServoDriver.h"

#include "item.h"
#include "AnalogLightMeter.h"
#include "ULN2003StepperMotor.h"
#include "MCP320X.h"
#include "Mpu6050.h"
#include "Si7021.h"
#include "sgp30.h"
#include "bmp280.h"
#include "tsl2561.h"
#include "lcd.h"

#include "AnalogMeterSimple.h"
#include "dustSensor.h"
#include "tmp36Sensor.h"

namespace pirobot {
const char TAG[] = "PiRobot";


/*
 *
 */
bool PiRobot::configure(const std::string& cfile){
    std::string conf = (cfile.empty() ? get_configuration() : cfile);
    bool result = true;

    if(conf.empty()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " No Robot configuration");
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Loading Robot hardware configuration ...");
    const std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(conf));
    if(!fl->is_success()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + "Could not load file " + fl->get_name() + " Error: " + std::to_string(fl->get_err()));
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Parse JSON from: " + conf);
    std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>(fl);
    if(!cjson->is_succeess()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not parse JSON: " + cjson->get_error());
        return false;
    }

    try{
        auto version = cjson->get_attr_string_def(cjson->get(), "version", "");
        auto real_world = cjson->get_attr_def<bool>(cjson->get(), "real_world", true);
        m_debug_data_folder = cjson->get_attr_string_def(cjson->get(), "debug_data_folder", "/var/log/pi-robot/");

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

        if(cjson->contains("providers"))
        {
            load_providers(cjson);
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No provider information");

        /*
        // Create GPIO
        */
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ------------ Load GPIOs ------------");
        if(cjson->contains("gpios")){
            load_gpios(cjson);
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No GPIO information");

        /*
        // Create Items
        */
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ------------ Load Items ------------");


        if(cjson->contains("items")){

            /*
            * Detect GPIO name
            */

            const auto json_items = cjson->get_array(std::string("items"));
            auto json_item = cjson->get_first(json_items);
            while(json_item){

                auto item_type = cjson->get_attr_string(json_item, "type");
                auto item_name = cjson->get_attr_string(json_item, "name");
                auto item_comment = cjson->get_attr_string_def(json_item, "comment", "");

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
                        if(!cjson->contains("gpio", json_item)){
                            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Item configuration. GPIO information is absent.");
                            throw std::runtime_error(std::string(" Invalid Item configuration. GPIO information is absent."));
                        }

                        load_item_one_gpio(cjson, json_item, itype, item_name, item_comment);
                    }//Single GPIO based Items
                    break;

                    case item::ItemTypes::ULN2003Stepper:
                    {
                            /*
                            DCMotor parameters: Name, Comment, [GPIO provider, PIN]
                            [0]=PHASE_0, [1]=PHASE_1, [2]=PHASE_2, [3]=PHASE_3
                            */
                            std::string gpio_phase_0_name = f_get_gpio_name(cjson, json_item, "gpio_phase_0", item_name);
                            std::string gpio_phase_1_name = f_get_gpio_name(cjson, json_item, "gpio_phase_1", item_name);
                            std::string gpio_phase_2_name = f_get_gpio_name(cjson, json_item, "gpio_phase_2", item_name);
                            std::string gpio_phase_3_name = f_get_gpio_name(cjson, json_item, "gpio_phase_3", item_name);

                            auto direction = f_get_motor_direction(cjson, json_item, item_name);

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
                            std::string gpio_rs_name = f_get_gpio_name(cjson, json_item, "gpio_rs", item_name);
                            std::string gpio_enable_name = f_get_gpio_name(cjson, json_item, "gpio_enable", item_name);

                            std::string gpio_data_4_name = f_get_gpio_name(cjson, json_item, "gpio_data_4", item_name);
                            std::string gpio_data_5_name = f_get_gpio_name(cjson, json_item, "gpio_data_5", item_name);
                            std::string gpio_data_6_name = f_get_gpio_name(cjson, json_item, "gpio_data_6", item_name);
                            std::string gpio_data_7_name = f_get_gpio_name(cjson, json_item, "gpio_data_7", item_name);

                            std::string gpio_backlite_name = f_get_gpio_name(cjson, json_item, "gpio_backlite", item_name);

                            auto lines  =  cjson->get_attr_def<int>(json_item, "lines", 1);
                            auto bitmode  =  cjson->get_attr_def<int>(json_item, "bitmode", 4);
                            auto dots  =  cjson->get_attr_def<int>(json_item, "dots", 8);

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
                        auto ad_convertor = cjson->get_attr_string(json_item, "ad_convertor");
                        auto analog_input_index = cjson->get_attr<int>(json_item, "analog_input_index");

                        auto debug_mode  =  cjson->get_attr_def<bool>(json_item, "debug", false);
                        auto debug_buffer_size  =  cjson->get_attr_def<int>(json_item, "debug_buffer_size", 2048);
                        auto value_diff_for_event  =  cjson->get_attr_def<int>(json_item, "value_diff_for_event", 0);

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

                    case item::ItemTypes::AnalogMeterSimple:
                /*
                * Universal simple Analod meter.
                */
                    {
                        auto ad_convertor = cjson->get_attr_string(json_item, "ad_convertor");

                        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Item: " + item_name + " AD conv: " + ad_convertor);

                        items_add(item_name, std::make_shared<pirobot::analogmeter::AnalogMeterSimple>(
                                std::static_pointer_cast<pirobot::analogdata::AnalogDataProviderItf>(std::static_pointer_cast<pirobot::mcp320x::MCP320X>(get_item(ad_convertor))),
                                item_name,
                                item_comment
                            ));

                        auto ameter = std::static_pointer_cast<pirobot::analogmeter::AnalogMeterSimple>(get_item(item_name));

                        const auto json_pins = cjson->get_array(json_item, std::string("pins"));
                        auto meter = cjson->get_first(json_pins);
                        while(meter){
                            auto pin  =  cjson->get_attr<int>(meter, "pin");
                            auto ameter_type  =  cjson->get_attr_string_def(meter, "type", "simple");
                            std::string ameter_name = ameter_type + std::to_string(pin);

                            if(ameter_type == "simple"){
                                ameter->add_receiver(pin, std::make_shared<analogdata::AnalogDataReceiverItf>(ameter_name, pin));
                            }
                            else if(ameter_type == "dustsensor"){
                                std::string gpio_name = f_get_gpio_name(cjson, meter, "gpio", item_name);
                                ameter->add_receiver(pin, std::make_shared<item::dustsensor::DustSensor>(get_gpio(gpio_name), pin, ameter_name));
                            }
                            else if(ameter_type == "tmp36"){
                                ameter->add_receiver(pin, std::make_shared<item::tmp36sensor::Tmp36Sensor>(pin, ameter_name));
                            }

                            meter = cjson->get_next(meter);
                        }
                    }
                    break;

                    case item::ItemTypes::BLINKER:
                /*
                * BLINKER parameters: Name, SUB.ITEM=LED
                */
                    {
                        load_item_one_gpio(cjson, json_item, itype, item_name, item_comment);
                    }
                    break;
                    /*
                    * MPU-6050 Accelerometer + Gyro
                    */
                    case item::ItemTypes::MPU6050:
                    {
                        auto i2c_provider = std::static_pointer_cast<pirobot::i2c::I2C>(get_provider("I2C"));
                        auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(json_item, "i2c_addr", MPU6050_I2C_ADDRESS);
                        auto loop_delay  = cjson->get_attr_def<unsigned int>(json_item, "delay", 100);

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

                        auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(json_item, "i2c_addr", pirobot::item::Bmp280::s_i2c_addr); //0x76
                        auto smode = cjson->get_attr_string_def(json_item, "mode", "FORCED");
                        auto pressure_oversampling = (uint8_t)cjson->get_attr_def<int>(json_item, "pressure_oversampling", 1);
                        auto temperature_oversampling = (uint8_t)cjson->get_attr_def<int>(json_item, "temperature_oversampling", 0xFF);
                        auto standby_time = (uint8_t)cjson->get_attr_def<int>(json_item, "standby_time", 5);
                        auto filter = (uint8_t)cjson->get_attr_def<int>(json_item, "filter", 0);
                        auto spi = cjson->get_attr_def<int>(json_item, "SPI", 0);
                        auto spi_channel  =  cjson->get_attr_def<int>(json_item, "spi_channel", 0);

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
                        auto i2c_addr = (uint8_t)cjson->get_attr_def<int>(json_item, "i2c_addr", pirobot::item::Tsl2561::s_i2c_addr); //0x39

                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item: " + item_name + " Comment: " + item_comment);
                        items_add(item_name, std::make_shared<pirobot::item::Tsl2561>(item_name, i2c_provider, i2c_addr, item_comment));
                    }
                    break;

                    /*
                    *  SPI based LED stripe controller
                    */
                    case item::ItemTypes::SLEDCRTLSPI:
                    {
                        load_sled(cjson, json_item, itype, item_name, item_comment);
                    }

                }//Item types

                json_item = cjson->get_next(json_item);
            }//Items collection loading
        }
        else
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No Items information");
    }
    catch(std::runtime_error& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
        result = false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Robot configuration is finished Result: " + std::to_string(result));

    //print loaded configuration
    printConfig();

    return result;
}

std::string PiRobot::f_get_gpio_name(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson,
                        const piutils::cjson_wrap::cj_obj object,
                        const std::string& gpio_object_name,
                        const std::string& item_name){

    auto gpio_object = cjson->get_object(object, gpio_object_name);

    //
    // If GPIO has his own name - use it without checking provider and PIN
    //
    if(cjson->contains("name", gpio_object)){
        return cjson->get_attr_string(gpio_object, "name");
    }

    auto gpio_provider = cjson->get_attr_string(gpio_object, "provider");
    auto gpio_pin = cjson->get_attr<int>(gpio_object, "pin");

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Item Name: " + item_name + " GPIO provider: " + gpio_provider + " Pin:" + std::to_string(gpio_pin));

    const auto provider = get_provider(gpio_provider);
    if(provider->get_ptype() != provider::PROVIDER_TYPE::PROV_GPIO){
        const std::string msg = " Invalid provider type. " + gpio_provider + " Is not GPIO provider.";
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + msg);
        throw std::runtime_error(msg);
    }

    return gpio::Gpio::get_gpio_name(gpio_provider, gpio_pin);
}

item::MOTOR_DIR PiRobot::f_get_motor_direction(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson,const piutils::cjson_wrap::cj_obj object, const std::string& item_name){
    std::string direction_name = cjson->get_attr_string_def(object, "direction", "CLOCKWISE");

    if((direction_name != "CLOCKWISE") && (direction_name != "COUTERCLOCKWISE")){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid Motor direction value: " + direction_name + " for " + item_name);
        throw std::runtime_error(std::string(" Invalid Motor direction value: ") + direction_name + " for " + item_name);
    }

    return (direction_name == "CLOCKWISE" ? item::MOTOR_DIR::DIR_CLOCKWISE : item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
}


}//namespace
