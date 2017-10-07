/*
 * PiRobotPrj1.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: Denis Kudia
 */
#include "PiRobotPrj1.h"
#include "logger.h"

namespace spi_test {

PiRobotPrj1::PiRobotPrj1(const bool real_world) : pirobot::PiRobot(real_world) {
    // TODO Auto-generated constructor stub

}

PiRobotPrj1::~PiRobotPrj1() {
    // TODO Auto-generated destructor stub
}

/*
 * Build In
 * GPIO 0 - 17
 * GPIO 1 - 18
 * GPIO 10 - CE0
 * GPIO 11 - CE1
 */
bool PiRobotPrj1::configure(){

    /*
    * Providers
    */
    add_provider("SPI", "SPI. One channel");
    

    /*
    * GPIOs
    */
    if(is_real_world())
        add_gpio("SIMPLE", pirobot::gpio::GPIO_MODE::OUT, 10);
    else
        add_gpio("SIMPLE", pirobot::gpio::GPIO_MODE::OUT, 0);
    
    // ****************** Items ****************

    //Analog/Digatal converter MCP320X
    pirobot::item::ItemConfig mcp_3208 = {pirobot::item::ItemTypes::AnlgDgtConvertor, 
        "MCP3208", "MCP3208", 
        {
            std::make_pair("SIMPLE", (is_real_world() ? 10 : 0))
        }
    };
    add_item(mcp_3208);

    pirobot::item::ItemConfig light_meter1 = {pirobot::item::ItemTypes::AnalogMeter, "LightMeter_1", "LightMeter", {}, "MCP3208", 0, 10};
    add_item(light_meter1);
    /*
    pirobot::item::ItemConfig light_meter2 = {pirobot::item::ItemTypes::AnalogMeter, "LightMeter_2", "LightMeter", {}, "MCP3208", 1};
    add_item(light_meter2);
    pirobot::item::ItemConfig light_meter3 = {pirobot::item::ItemTypes::AnalogMeter, "LightMeter_3", "LightMeter", {}, "MCP3208", 2};
    add_item(light_meter3);
    */

    printConfig();

    logger::log(logger::LLOG::NECECCARY, __func__, "SPI test configuration is finished");
    return true;
}


} /* namespace project1 */
