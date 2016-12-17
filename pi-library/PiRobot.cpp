/*
 * PiRobot.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <unistd.h>
#include <iostream>
#include <functional>

#include "PiRobot.h"
#include "logger.h"

#include "GpioProviderFake.h"
#include "item.h"
#include "led.h"
#include "button.h"

namespace pirobot {

PiRobot::PiRobot(const bool realWorld)
	: m_realWorld(realWorld),
	  stm_notification(nullptr)
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
 * Get GPIO by ID
 */
std::shared_ptr<gpio::Gpio> PiRobot::get_gpio(const int id) const{
	auto pgpio = this->gpios.find(id);
	if(pgpio == gpios.end()){
		logger::log(logger::LLOG::ERROR, __func__, " Absent requested GPIO with ID " + std::to_string(id));
		throw std::runtime_error(std::string("No GPIO with ID: ") + std::to_string(id));
	}
	return pgpio->second;
}

/*
 * Get Item by name
 */
std::shared_ptr<item::Item> PiRobot::get_item(const std::string& name) const{
	auto item = this->items.find(name);
	if(item == items.end()){
		logger::log(logger::LLOG::ERROR, __func__, " Absent requested GPIO with ID " + name);
		throw std::runtime_error(std::string("No GPIO with ID: ") + name);
	}
	return item->second;

}

/*
 *
 */
bool PiRobot::start(){
	logger::log(logger::LLOG::NECECCARY, __func__, "Robot is starting..");
	std::map<const std::string, std::shared_ptr<item::Item>>::iterator it;

	for(it = this->items.begin(); it != this->items.end(); ++it){
		logger::log(logger::LLOG::NECECCARY, __func__, "Initialize " + it->first);

		/*
		 * Set callback for Button
		 * TODO: Add another types if needed
		 */
		if(it->second->type() == item::ItemTypes::BUTTON){
			it->second->notify = std::bind(&PiRobot::notify_stm,
					this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}

		const bool res = it->second->initialize();
		if(!res){
			logger::log(logger::LLOG::ERROR, __func__, "Initialization failed " + it->first);
			return false;
			/*TODO: Throw Exception and exit? */
		}
        //(std::dynamic_pointer_cast<item::Led>(it->second))->On();
	}

	logger::log(logger::LLOG::NECECCARY, __func__, "Robot is started ");
	return true;
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
/*
	std::shared_ptr<gpio::GpioProvider> provider(new gpio::GpioProviderFake());

	this->gpios[1] = std::shared_ptr<gpio::Gpio>(new gpio::Gpio(1, gpio::GPIO_MODE::OUT, provider));
	this->gpios[2] = std::shared_ptr<gpio::Gpio>(new gpio::Gpio(2, gpio::GPIO_MODE::IN, provider));

	this->items[std::string("LED_1")] = std::shared_ptr<item::Item>(new item::Led(gpios.at(1), "LED_1", "LED first"));
	this->items[std::string("BTN_1")] = std::shared_ptr<item::Item>(new item::Button(gpios.at(2), "BTN_1", "Button for LED_1"));
*/
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

/*
 * Notify State Machine about Item event
 */
void PiRobot::notify_stm(int itype, std::string& name, void* data){
	if(stm_notification != nullptr){
		stm_notification(itype, name, data);
	}
	else
		logger::log(logger::LLOG::NECECCARY, __func__, " Received notification Item Type: " + std::to_string(itype) + " Name: " + name);
}


} /* namespace pirobot */
