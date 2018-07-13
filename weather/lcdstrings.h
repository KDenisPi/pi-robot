/*
 * lcdstrings.h
 *
 *  Created on: June 12, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_LCDSTRINGS_H_
#define WEATHER_LCDSTRINGS_H_

#include <string>
#include <map>

namespace weather {

enum StrID {
    Starting = 0,
    Warming,
    Ip4Address,
    Ip6Address,
    Finishing,
    Good,
    Moderate,
    Poor,
    Dangerous,
    Line1,
    Line2,
    Good_ru,
    Moderate_ru,
    Poor_ru,
    Dangerous_ru
};

class LcdStrings {
public:
    LcdStrings(const std::string& language="en") : _lang(language) {
        load();
    }

    virtual ~LcdStrings() {}

    const std::string& get(const int id) const {
        try{
            return _msg.at(id);
        }
        catch(std::out_of_range&){
        }
        return _outofrange;
    }

    //Load string list
    void load(){
        _msg[StrID::Starting] = "Starting...";
        _msg[StrID::Warming] = "Warming...";
        _msg[StrID::Ip4Address] = "IP4 address:";
        _msg[StrID::Ip6Address] = "IP6 address:";
        _msg[StrID::Finishing] = "Finishing...";
        _msg[StrID::Good] = "Good";
        _msg[StrID::Moderate] = "Not bad";
        _msg[StrID::Poor] = "Poor";
        _msg[StrID::Dangerous] = "%s Dangerous";
        _msg[StrID::Line1] = "T:%.0f%c CO2:%s";
        _msg[StrID::Line2] = "H:%.0f%c P:%.0fmm";
        _msg[StrID::Good_ru] = "Хороший";
        _msg[StrID::Moderate_ru] = "Нормальный";
        _msg[StrID::Poor_ru] = "Повышенный";
        _msg[StrID::Dangerous_ru] = "Опасный";
    }

private:
    std::string _lang;
    std::map<int, std::string> _msg;
    std::string _outofrange = "Out of range";
};

}//namespace weather
#endif
