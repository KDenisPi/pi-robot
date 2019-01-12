/*
 * item.h
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_ITEM_H_
#define PI_LIBRARY_ITEM_H_

#include <functional>
#include <array>


#include "gpio.h"

namespace pirobot {
namespace item {

enum ItemTypes{
    UNKNOWN = 0,
    LED,
    BUTTON,
    SERVO,
    TILT_SWITCH,
    STEPPER,
    DCMotor,
    DRV8835,
    BLINKER,
    AnlgDgtConvertor,
    AnalogLightMeter,
    ULN2003Stepper,
    MPU6050,
    SI7021,
    SGP30,
    BMP280,
    TSL2561,
    LCD,
    SLED,
    SLEDCRTLSPI,
    SLEDCRTLPWM,
    LAST_ITEM
};

enum BUTTON_STATE{
    BTN_NOT_PUSHED = 0,
    BTN_PUSHED = 1
};

enum MOTOR_DIR {
    DIR_CLOCKWISE = 0,
    DIR_COUTERCLOCKWISE = 1
};

enum GENERAL_NTFY {
    GN_DONE
};

struct ItemConfig{
    ItemTypes type;
    std::string name;
    std::string comment;

    std::array<std::pair<std::string, int>,5> gpios;

    std::string sub_item;
    int index;
    int value_1;
    int value_2;
};

class Item {
public:
    //Constructor for general Items
    Item( const std::string& name,
          const std::string& comment,
          int itype) :
    m_name(name),
    m_comment(comment),
    m_type(itype),
    notify(nullptr),
    m_debug(false)
    {};

    //Constructor for GPIO base Items
    Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio, int itype) : Item(gpio, "", "", itype) {};

    Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
          const std::string& name,
          const std::string& comment,
          int itype
        ): Item(name, comment, itype)
    {
        m_gpio = gpio;
    }

    virtual ~Item() {}

    //Item initialization function
    virtual bool initialize() {return true;}
    //Item stop  function
    virtual void stop() {}
    //activate debug functionality for the item
    virtual void activate_debug() {}

    // Parameters:
    // dest_type - type of destination. "file" will be supported only by now
    virtual void unload_debug_data(const std::string& dest_type, const std::string& destination) {}
    inline bool const is_debug() const {return m_debug;}

    inline void set_name(const std::string& name) {m_name = name;}
    inline void set_comment(const std::string& comment) {m_comment = comment;}

    inline const std::string name() const { return m_name; }
    inline const std::string comment() const { return m_comment;}
    inline const int type() const { return m_type;}

    //returm item state
    virtual const bool is_ok() const {return true;}

    virtual const std::string to_string() {
        return name();
    }

    virtual const std::string printConfig() = 0;

    inline const std::shared_ptr<pirobot::gpio::Gpio> get_gpio() const {
        return m_gpio;
    }

    //
    // Static function for working with item type
    //
    static const std::string type_name(const int typen) {
        return std::string(ItemNames[typen]);
    }

    static const item::ItemTypes type_by_name(const std::string& name){
        for(int i = 0; i <  item::ItemTypes::LAST_ITEM; i++){
            if(name == ItemNames[i])
                return (item::ItemTypes)i;
        }

        return item::ItemTypes::UNKNOWN;
    }

    static const char* ItemNames[];

    //Should we set callback function during initialization for this Item or not
    inline bool have_notify(){
        if(m_type == ItemTypes::BUTTON ||
            m_type == ItemTypes::TILT_SWITCH ||
            m_type == ItemTypes::BLINKER ||
            m_type == ItemTypes::AnalogLightMeter ||
            m_type == ItemTypes::STEPPER) {
                return true;
        }
        return false;
    }

    //callback function for upper level notification
    std::function<void(int, std::string&, void*)> notify;

private:
    std::string m_name;
    std::string m_comment;
    int m_type;

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio;

protected:
    bool m_debug;
};

}/* namespace item*/
}/* namespace pirobot */


#endif /* PI_LIBRARY_ITEM_H_ */
