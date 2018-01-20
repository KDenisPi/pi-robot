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
    virtual bool configure(const std::string& cfile);

    /*
     *
     */
    bool start();

    /*
     *
     */
    void stop();

    /*
     * Get Item by name
     */
    std::shared_ptr<item::Item> get_item(const std::string& name) const noexcept(false);

private:
    /*
     * Get GPIO by absolute ID
     * Get GPIO by provider name and ID
     */
    std::shared_ptr<gpio::Gpio> get_gpio(const std::string& name) const noexcept(false);
    std::shared_ptr<gpio::Gpio> get_gpio(const std::string provider, const int id)  const noexcept(false);

    inline const std::vector<std::string> get_items() const {
        std::vector<std::string> keys;
        for(auto imap: items)
            keys.push_back(imap.first);
        return keys;
    }

    void items_add(const std::string& name, const std::shared_ptr<item::Item>& item){
        items[name] = item;
    }

    const std::string get_gpio_name(const std::string& provider_name, const int pin) const {
        std::string gpio_ = provider_name + "_" + std::to_string(pin);
        return gpio_;
    }

    /*
    * Get provider by name
    */
    std::shared_ptr<provider::Provider> get_provider(const std::string& name) const noexcept(false);
    
    /*
    * Check if provider exists by name
    */
    bool is_provider(const std::string& name) const{
        auto provider = this->providers.find(name);
        return (provider != providers.end());
    }

    /*
    * Create GPIO for the provider. Provider should created before using add_provider function
    */
    void add_gpio(const std::string& name, const std::string& provider_name, 
        const pirobot::gpio::GPIO_MODE gpio_mode, const int pin) noexcept(false);
    
    /*
    *
    */
    void add_item(const pirobot::item::ItemConfig& iconfig)  noexcept(false);   

public:
    /*
     *
     */
    void notify_stm(int itype, std::string& name, void* data);
    
    std::function<void(int, std::string&, void*)> stm_notification;

    void printConfig();

    /*
    *
    */
    const bool is_real_world() const { return m_realWorld;}
    
   /*
   *
   */
   void set_configuration(const std::string& conf_file){
    m_configuration = conf_file;
   }

   const std::string& get_configuration() const{
       return m_configuration;
   }

private:
    //
    //
    //
    void gpios_add(const std::string& name, const std::shared_ptr<gpio::Gpio> gpio){
        gpios[name] = gpio;
    }

    void set_real_world(const bool real_world){
        m_realWorld = real_world;
    }

private:
    bool m_realWorld;
    std::recursive_mutex mutex_sm;

    std::string m_configuration; //full defined path to the hardware configuration file 
    
    std::map <std::string,
        std::shared_ptr<gpio::Gpio>,
        std::less<std::string>,
        std::allocator<std::pair<const std::string, std::shared_ptr<gpio::Gpio>> >
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
