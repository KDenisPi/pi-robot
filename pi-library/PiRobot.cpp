/*
 * PiRobot.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <unistd.h>
#include <iostream>

#include "PiRobot.h"
#include "logger.h"

#include "GpioProviderFake.h"
#include "item.h"
#include "led.h"
#include "button.h"

namespace pirobot {

PiRobot::PiRobot(const bool realWorld)
	: m_realWorld(realWorld)
{
	logger::log(logger::LLOG::NECECCARY, __func__, std::string("Started. Real world? ") + (realWorld ? " TRUE" : " FALSE"));

	if(realWorld) //Under PI
	{
		int wiPi = wiringPiSetup();
	}
}

PiRobot::~PiRobot() {
	logger::log(logger::LLOG::NECECCARY, __func__, "Started");
	/*
	 * Delete all objects
	 */
	logger::log(logger::LLOG::NECECCARY, __func__, "Finished");
}

/*
 *
 */
const std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const int id){
	auto pgpio = this->gpios.find(id);
	if(pgpio == gpios.end()){
		logger::log(logger::LLOG::ERROR, __func__, " Absent requested GPIO with ID " + std::to_string(id));
	}
	return pgpio->second;
}

/*
 *
 */
void PiRobot::start(){
	logger::log(logger::LLOG::NECECCARY, __func__, "Robot is starting..");

	std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;

	for(it = this->items.begin(); it != this->items.end(); ++it){
		logger::log(logger::LLOG::NECECCARY, __func__, "Initialize " + it->first);

		const bool res = it->second->initialize();
		if(!res){
			logger::log(logger::LLOG::ERROR, __func__, "Initialization failed " + it->first);
			/*TODO: Throw Exception and exit? */
		}
                (std::dynamic_pointer_cast<item::Led>(it->second))->On();
	}

	logger::log(logger::LLOG::NECECCARY, __func__, "Robot is started ");
}

/*
 *
 */
void PiRobot::stop(){
	logger::log(logger::LLOG::NECECCARY, __func__, "Robot is stopping..");

	std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
	for(it = this->items.begin(); it != this->items.end(); ++it){
		logger::log(logger::LLOG::NECECCARY, __func__, "Stopping " + it->first);
		it->second->stop();
	}

	logger::log(logger::LLOG::NECECCARY, __func__, "Robot was stopped");
}

/*
 *
 */
bool PiRobot::configure(){
	logger::log(logger::LLOG::NECECCARY, __func__, "Robot configuration is starting..");

	std::shared_ptr<gpio::GpioProvider> provider(new gpio::GpioProviderFake());

	this->gpios[1] = std::shared_ptr<gpio::Gpio>(new gpio::Gpio(1, gpio::GPIO_MODE::OUT, provider));
	this->gpios[2] = std::shared_ptr<gpio::Gpio>(new gpio::Gpio(2, gpio::GPIO_MODE::IN, provider));

	this->items[std::string("LED_1")] = std::shared_ptr<item::Item>(new item::Led(gpios.at(1), "LED_1", "LED first"));
	this->items[std::string("BTN_1")] = std::shared_ptr<item::Item>(new item::Button(gpios.at(2), "BTN_1", "Button for LED_1"));

	logger::log(logger::LLOG::NECECCARY, __func__, "Robot configuration is finished");
	return true;
}

void PiRobot::printConfig(){
        std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;
	std::cout << "-------------- configuration ---------------------------" << std::endl;
        for(it = this->items.begin(); it != this->items.end(); ++it){
                std::cout << it->first << " " << it->second->printConfig() << std::endl;
        }
}

} /* namespace pirobot */
