/*
 * lcd.h
 *
 *  Created on: May 27, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_LCD_H_
#define PI_LIBRARY_LCD_H_

#include <thread>

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
        m_gpio_backlite(backlite),
        _lines( lines==1 ? LCD_2LINE : LCD_1LINE),
        _bitmode(bitmode==4 ? LCD_4BITMODE : LCD_8BITMODE),
        _dots(dots==8 ? LCD_5x8DOTS : LCD_5x10DOTS)
    {
        m_gpio_data[4] = data_4;
        m_gpio_data[5] = data_5;
        m_gpio_data[6] = data_6;
        m_gpio_data[7] = data_7;

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

        // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
        // according to datasheet, we need at least 40ms after power rises above 2.7V
        // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        //switch to command mode
        m_gpio_rs->Low();
        m_gpio_enable->Low();
        if(m_gpio_rw){
            m_gpio_rw->Low();
        }

        //use 4-bit mode
        if(_bitmode == LCD_4BITMODE){
            write4bits(0x03); //0011 - command set + 8-bit
            std::this_thread::sleep_for(std::chrono::microseconds(4500));

            write4bits(0x03); //0011 - command set + 8-bit
            std::this_thread::sleep_for(std::chrono::microseconds(4500));

            write4bits(0x03); //0011 - command set + 8-bit
            std::this_thread::sleep_for(std::chrono::microseconds(150));

            write4bits(0x02); //0010 - command set + 4-bit
        }
        else{
            write8bits(LCD_FUNCTIONSET | _displayfunction);
            std::this_thread::sleep_for(std::chrono::microseconds(4500));

            write8bits(LCD_FUNCTIONSET | _displayfunction);
            std::this_thread::sleep_for(std::chrono::microseconds(4500));

            write8bits(LCD_FUNCTIONSET | _displayfunction);
            std::this_thread::sleep_for(std::chrono::microseconds(150));
        }

        command(_displayfunction);
    }

    //
    //Display contril commands
    //

    //Display On
    void diplay_on(){
        _displaycontrol |= LCD_DISPLAYON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }

    //Display Off
    void display_off(){
        _displaycontrol &= ~LCD_DISPLAYON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }

    //Cursor On
    void cursor_on(){
        _displaycontrol |= LCD_CURSORON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }

    //Cursor Off
    void cursor_off(){
        _displaycontrol &= ~LCD_CURSORON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }

    //Cursor blink On
    void cursor_blink_on(){
        _displaycontrol |= LCD_BLINKON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }

    //Cursor blink Off
    void cursor_blink_off(){
        _displaycontrol &= ~LCD_BLINKON;
        command(LCD_FUNCTIONSET | _displaycontrol);
    }


private:

    void command(const uint8_t cmd){

        m_gpio_rs->Low(); //set 0 - command

        if(_bitmode == LCD_4BITMODE){
            write4bits(cmd >> 4);
            write4bits(cmd&0x0F);
        }
        else
            write8bits(cmd);
    }

    //
    //Pulse enable bit
    //
    void pulse_enable(){
        m_gpio_enable->Low();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        m_gpio_enable->High();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        m_gpio_enable->Low();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    //
    //Write 4-bits
    //
    void write4bits(const uint8_t value){

        //
        //I2C based providers
        //It will not work if we have more than one provider here
        //
        //TODO: Check providers and switch to pin based provider then
        //
        if(m_gpio_data[7]->get_provider_type() == pirobot::gpio::GPIO_PROVIDER_TYPE::PROV_MCP23017 ||
            m_gpio_data[7]->get_provider_type() == pirobot::gpio::GPIO_PROVIDER_TYPE::PROV_MCP23008){

            auto provider = std::static_pointer_cast<pirobot::gpio::GpioProviderMCP230xx>(m_gpio_data[7]->get_provider());
            uint8_t cur_value = provider->dgtRead8(m_gpio_data[7]->getPin());

            for(int i=0; i<4; i++){
                int pin = m_gpio_data[i+4]->getPin();
                uint8_t mask = (1 << pin); //we use GPIOs 4-7 here
                cur_value &= ~mask; //clear current value for bit
                cur_value |= ((value >> i) & 0x1) << pin; //set value
            }

            provider->dgtWrite8(cur_value, m_gpio_data[7]->getPin());
            //pulse enable pin
            pulse_enable();

        }
        else{
            //
            //If we connected to the separate pins
            //
            for(int i=0; i<4; i++){
                if((value >> i) & 0x1) m_gpio_data[i]->High();
                    else m_gpio_data[i]->Low();
            }
            //pulse enable pin
            pulse_enable();
        }
    }

    //
    //Write 8-bits
    //
    void write8bits(const uint8_t value){
        //
        //I2C based providers
        //It will not work if we have more than one provider here
        //
        //TODO: Check providers and switch to pin based provider then
        //
        if(m_gpio_data[7]->get_provider_type() == pirobot::gpio::GPIO_PROVIDER_TYPE::PROV_MCP23017 ||
            m_gpio_data[7]->get_provider_type() == pirobot::gpio::GPIO_PROVIDER_TYPE::PROV_MCP23008){

            auto provider = std::static_pointer_cast<pirobot::gpio::GpioProviderMCP230xx>(m_gpio_data[7]->get_provider());
            provider->dgtWrite8(value, m_gpio_data[7]->getPin());
            //pulse enable pin
            pulse_enable();
        }
        else{
            //
            //If we connected to the separate pins
            //
            for(int i=0; i<8; i++){
                if((value >> i) & 0x1) m_gpio_data[i]->High();
                    else m_gpio_data[i]->Low();
            }
            //pulse enable pin
            pulse_enable();
        }
    }

private:
    uint8_t _lines;
    uint8_t _bitmode;
    uint8_t _dots;

    uint8_t _displayfunction;
    uint8_t _displaycontrol;  //cursor on/off, cursor blink on/off, display on/off

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_rs;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_rw;
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_enable;

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_backlite; //if we use Ardafuit module we have special backlite control pin

    std::shared_ptr<pirobot::gpio::Gpio> m_gpio_data[8];
};

}//namespace lcd
}//namespace item
}//namespace pirobot

#endif