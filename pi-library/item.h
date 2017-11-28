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
    LED = 1,
    BUTTON = 2,
    SERVO = 3,
    TILT_SWITCH = 4,
    STEPPER = 5,
    DCMotor = 6,
    DRV8835 = 7,
    BLINKER = 8,
    AnlgDgtConvertor = 9,
    AnalogMeter = 10,
    ULN2003Stepper = 11
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
    Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio, int itype) :
    m_gpio(gpio),
    m_name(),
    m_comment(),
    m_type(itype),
    notify(nullptr),
    m_debug(false)
    {};

    Item( const std::string& name,
          const std::string& comment,
          int itype) :
    m_name(name),
    m_comment(comment),
    m_type(itype),
    notify(nullptr),
    m_debug(false)
    {};
    
    Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
          const std::string& name,
          const std::string& comment,
          int itype
        ):
        m_gpio(gpio),
        m_name(name),
        m_comment(comment),
        m_type(itype),
        notify(nullptr),
        m_debug(false)
    {};

    virtual ~Item() {};

    virtual bool initialize() {return true;};
    virtual void stop() {};

    virtual void activate_debug() {};
    // Parameters: 
    // dest_type - type of destination. "file" will be supported only by now
    virtual void unload_debug_data(const std::string& dest_type, const std::string& destination) {};
    bool const is_debug() const {return m_debug;}

    void set_name(const std::string& name) {m_name = name;}
    void set_comment(const std::string& comment) {m_comment = comment;}

    inline const std::string name() const { return m_name; }
    inline const std::string comment() const { return m_comment;}
    inline const int type() const { return m_type;}

    virtual const std::string to_string() {
        return name();
    }

    virtual const std::string printConfig() = 0;

    const std::shared_ptr<pirobot::gpio::Gpio> get_gpio() const {
        return m_gpio;
    }

    static const std::string type_name(const int typen) {
        return std::string(ItemNames[typen]); 
    }

    static const char* ItemNames[];

    inline bool have_notify(){
        if(m_type == ItemTypes::BUTTON || 
            m_type == ItemTypes::TILT_SWITCH ||
            m_type == ItemTypes::BLINKER ||
            m_type == ItemTypes::AnalogMeter ||
            m_type == ItemTypes::STEPPER) {
                return true;
        }
        return false;
    }

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
