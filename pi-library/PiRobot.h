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

#ifdef REAL_HARDWARE
#include <bcm_host.h>
#endif

#include "cJSON_wrap.h"

#include "provider.h"
#include "GpioProvider.h"
#include "gpio.h"
#include "item.h"

namespace pirobot {

using item_name_type = std::pair<std::string, item::ItemTypes>;

class PiRobot {
public:
    PiRobot();
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
    template<class T>
    const std::shared_ptr<T> get_item(const std::string& name)  const noexcept(false) {
        const auto item = this->items.find(name);
        if(item == items.end()){
            logger::log(logger::LLOG::ERROR, "PiRobot", std::string(__func__) + " Absent requested Item with ID " + name);
            throw std::runtime_error(std::string("No Item with ID: ") + name);
        }
        return std::static_pointer_cast<T>(item->second);

    }

    /*
    * Link Gpio and Item
    */
   void link_gpio_item(const std::string gpio_name, item_name_type item_info){
       auto gpio_item = gpio_items.find(gpio_name);
       if(gpio_item == gpio_items.end()){
           gpio_items[gpio_name] = std::make_shared<std::vector<item_name_type>>();
           gpio_items[gpio_name]->push_back(item_info);
       }
       else{
           gpio_item->second->push_back(item_info);
       }
   }

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
        const pirobot::gpio::GPIO_MODE gpio_mode,
        const int pin,
        const pirobot::gpio::PULL_MODE pull_mode = pirobot::gpio::PULL_MODE::PULL_OFF,
        const pirobot::gpio::GPIO_EDGE_LEVEL edge_level = pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE) noexcept(false);

    /*
    *
    */
    void add_item(const pirobot::item::ItemConfig& iconfig)  noexcept(false);

public:
    /*
     *
     */
    void notify_stm(int itype, std::string& name, void* data);

    //pointer to State Mashine notification function
    std::function<void(int, std::string&, void*)> stm_notification;

    /*
        Low level notification function (provider callback etc)
        Parameters:
            ptype - provider type
            pname - provider name
            pdata - provider data used for notification
    */
    void notify_low(const pirobot::provider::PROVIDER_TYPE ptype, const std::string& pname, std::shared_ptr<pirobot::provider::ProviderData> pdata);

    //pring configuration
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
    	logger::log(logger::LLOG::DEBUG, "PiRbt", std::string(__func__) + " Config: " + conf_file);
   }

   const std::string& get_configuration() const{
        logger::log(logger::LLOG::DEBUG, "PiRbt", std::string(__func__) + " Config: " + m_configuration);
        return m_configuration;
   }


    std::string f_get_gpio_name(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson, const piutils::cjson_wrap::cj_obj object, const std::string& gpio_object_name, const std::string& item_name);
    item::MOTOR_DIR f_get_motor_direction(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson,const piutils::cjson_wrap::cj_obj object, const std::string& item_name);


    bool load_providers(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson) noexcept(false);
    bool load_gpios(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson) noexcept(false);
    bool load_item_one_gpio(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson, const piutils::cjson_wrap::cj_obj json_item,
            item::ItemTypes itype, const std::string& item_name, const std::string& item_comment) noexcept(false);
    bool load_sled(const std::shared_ptr<piutils::cjson_wrap::CJsonWrap>& cjson, const piutils::cjson_wrap::cj_obj json_item,
            item::ItemTypes itype, const std::string& item_name, const std::string& item_comment) noexcept(false);

private:

    void set_real_world(const bool real_world){
        m_realWorld = real_world;
    }

    bool m_realWorld;
    std::mutex mutex_sm;

    std::string m_configuration; //full defined path to the hardware configuration file

    //Folder for saving measuremets oer components supported it
    std::string m_debug_data_folder;

    std::map <std::string,
        std::shared_ptr<gpio::Gpio>,
        std::less<std::string>,
        std::allocator<std::pair<const std::string, std::shared_ptr<gpio::Gpio>> >
    > gpios;

    //Map for detection GPIO by low level name (Provider + Pin)
    std::map <std::string,
        std::shared_ptr<gpio::Gpio>,
        std::less<std::string>,
        std::allocator<std::pair<const std::string, std::shared_ptr<gpio::Gpio>> >
    > gpios_low;

    //relation betwen GPIO and ITENS used this GPIO
    std::map <std::string, std::shared_ptr<std::vector<item_name_type>>> gpio_items;

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
