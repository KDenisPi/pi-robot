/*
 * button.h
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_BUTTON_H_
#define PI_LIBRARY_BUTTON_H_

#include <memory>
#include <cassert>

#include "item.h"
#include "Threaded.h"

namespace pirobot {
namespace item {


class Button: public Item, public piutils::Threaded {
public:

    /*
     * Constructor
     */
    Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
            const std::string name,
            const std::string comment,
            const BUTTON_STATE state = BUTTON_STATE::BTN_NOT_PUSHED,
            const gpio::PULL_MODE pullmode = gpio::PULL_MODE::PULL_DOWN, //gpio::PULL_MODE::PULL_UP,
            const int itype = ItemTypes::BUTTON);

    virtual ~Button();
    virtual bool initialize() override;
    virtual const std::string to_string() override;
    virtual const std::string printConfig() override;

    //Should we set callback function during initialization for this Item or not
    virtual bool is_notify() override{
        return true;
    }

    static void worker(Button* owner);
    virtual void stop() override;

    /*
    * Some providers return inverse values (for example MCP 23017)
    */
    const BUTTON_STATE get_state_by_level(gpio::SGN_LEVEL level)const {
      if(inverse_value)
        return (level == gpio::SGN_LEVEL::SGN_LOW ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED);

      return (level == gpio::SGN_LEVEL::SGN_HIGH ? BUTTON_STATE::BTN_PUSHED : BUTTON_STATE::BTN_NOT_PUSHED);
    }

    void set_state(const BUTTON_STATE state);
    const BUTTON_STATE state() { return m_state; }

private:
    gpio::PULL_MODE m_pullmode;
    BUTTON_STATE m_state; //
    bool inverse_value;
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_BUTTON_H_ */
