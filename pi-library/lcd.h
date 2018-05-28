/*
 * lcd.h
 *
 *  Created on: May 27, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_LCD_H_
#define PI_LIBRARY_LCD_H_

#include "item.h"

namespace pirobot {
namespace item {
namespace lcd {

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class Lcd : public Item {
public:
    // 4-bit mode constructor
    Lcd(const std::shared_ptr<pirobot::gpio::Gpio> rs,
        const std::shared_ptr<pirobot::gpio::Gpio> enb,
        const std::shared_ptr<pirobot::gpio::Gpio> data_4,
        const std::shared_ptr<pirobot::gpio::Gpio> data_5,
        const std::shared_ptr<pirobot::gpio::Gpio> data_6,
        const std::shared_ptr<pirobot::gpio::Gpio> data_7,
        const std::shared_ptr<pirobot::gpio::Gpio> backlite,
        const std::string name,
        const std::string comment,
        const uint8_t lines = LCD_2LINE,
        const uint8_t bitmode = LCD_4BITMODE,
        const uint8_t dots = LCD_5x8DOTS
    ) : Item(name, comment, ItemTypes::LCD), 
        m_gpio_rs(rs), 
        m_gpio_rw(nullptr), 
        m_gpio_enable(enb),
        m_gpio_data_4(data_4),
        m_gpio_data_5(data_5),
        m_gpio_data_6(data_6),
        m_gpio_data_7(data_7),
        m_gpio_backlite(backlite),
        _lines( lines==1 ? LCD_2LINE : LCD_1LINE),
        _bitmode(bitmode==4 ? LCD_4BITMODE : LCD_8BITMODE),
        _dots(dots==8 ? LCD_5x8DOTS : LCD_5x10DOTS)
    {

    }

    //TODO: Add interface for 8-bit mode and rw GPIO supported

    //
    virtual ~Lcd(){
        stop();
    }

    virtual const std::string printConfig() override {
        std::string conf =  name() + "\n" + " BitMode: " + (_bitmode==LCD_4BITMODE ? "4-Bit" : "8-Bit") +
            " Lines: " + (_lines==LCD_2LINE ? "2" : "1") + " Dots: " + (_dots==LCD_5x8DOTS ? "5x8" : "5x10");
        return conf;
    }

    //Item stop  function
    virtual void stop() override {
        //if we have Backlite pin then switch it Off
        if(m_gpio_backlite){
            m_gpio_backlite->Low();
        }

    }

    //Item initialization function
    virtual bool initialize() override {
        //if we have Backlite pin then switch it on
        if(m_gpio_backlite){
            m_gpio_backlite->High();
        }

        _displayfunction = _lines | _dots | _bitmode;

        //switch to command mode 
        m_gpio_rs->Low();

        m_gpio_enable->Low();
        if(m_gpio_rw){
            m_gpio_rw->Low();
        }

        // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
        // according to datasheet, we need at least 40ms after power rises above 2.7V
        // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
    }
   

private:
    uint8_t _lines;
    uint8_t _bitmode;
    uint8_t _dots;

    uint8_t _displayfunction;

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_rs;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_rw;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_enable;

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_backlite; //if we use Ardafuit module we have special backlite control pin

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_0;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_1;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_2;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_3;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_4;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_5;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_6;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data_7;
};

}//namespace lcd
}//namespace item
}//namespace pirobot

#endif