/*
 * PiRobot.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_PIROBOT_H_
#define PI_LIBRARY_PIROBOT_H_

#include <map>
#include <memory>
#include <functional>
#include <vector>
#include <mutex>

#include <wiringPi.h>

#include "provider.h"
#include "GpioProvider.h"
#include "gpio.h"
#include "item.h"

namespace pirobot {

class PiRobot {
public:
    PiRobot(const bool realWorld = false);
    virtual ~PiRobot();

    /*
     * Initialize components
     *
     * TODO: Read configuration from file
     */
    virtual bool configure();

    /*
     *
     */
    bool start();

    /*
     *
     */
    void stop();

    /*
     * Get GPIO by ID
     */
    std::shared_ptr<gpio::Gpio> get_gpio(const int id) const noexcept(false);

    /*
     * Get Item by name
     */
    std::shared_ptr<item::Item> get_item(const std::string& name) const noexcept(false);

    void gpios_add(int idx, const std::shared_ptr<gpio::Gpio> gpio){
        gpios[idx] = gpio;
    }

    inline const std::vector<int> get_gpios() const {
        std::vector<int> keys;
        for(auto imap: gpios)
            keys.push_back(imap.first);
        return keys;
    }

    inline const std::vector<std::string> get_items() const {
        std::vector<std::string> keys;
        for(auto imap: items)
            keys.push_back(imap.first);
        return keys;
    }

    void items_add(const std::string& name, const std::shared_ptr<item::Item>& item){
        items[name] = item;
    }

    /*
    * Create configuration functions
    *
    */
    void add_provider(const std::string& type, const std::string& name) noexcept(false);
    std::shared_ptr<provider::Provider> get_provider(const std::string& name) const noexcept(false);
    
    /*
    * Create GPIO for the provider. Provider should created before using add_provider function
    */
    void add_gpio(const std::string& provider_name, 
        const pirobot::gpio::GPIO_MODE gpio_mode, const int pin) noexcept(false);

    void printConfig();

    /*
     *
     */
    void notify_stm(int itype, std::string& name, void* data);
    
    std::function<void(int, std::string&, void*)> stm_notification;

private:
    bool m_realWorld;
    std::recursive_mutex mutex_sm;
    
    const bool is_real_world() const { return m_realWorld;}

    std::map <int,
        std::shared_ptr<gpio::Gpio>,
        std::less<int>,
        std::allocator<std::pair<const int, std::shared_ptr<gpio::Gpio>> >
    > gpios;

    std::map <std::string,
        std::shared_ptr<item::Item>,
        std::less<std::string>,
        std::allocator<std::pair<const std::string, std::shared_ptr<item::Item>> >
    > items;

    std::map <std::string,
        std::shared_ptr<provider::Provider>,
        std::less<std::string>,
        std::allocator<std::pair<const std::string, std::shared_ptr<provider::Provider>> >
    > providers;

};

} /* namespace pirobot */

#endif /* PI_LIBRARY_PIROBOT_H_ */
