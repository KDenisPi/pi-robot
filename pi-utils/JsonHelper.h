/*
 * JsonHelper.h
 *
 *  Created on: Jan 18, 2018
 *      Author: Denis Kudia
 */

#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <string>
#include "jsoncons/json.hpp"
#include "logger.h"

namespace jsonhelper {

/*
    Get attribute value and use defauilt value if absent
*/
template<class T>
T get_attr(const jsoncons::json& object, const std::string& name, const T& attr_default){
        return (object.has_key(name) ? object[name].as<T>() : attr_default);
};

/*
    Get value for mandatory attribute and raise exception if attribut absent.
*/
template<class T>
T get_attr_mandatory(const jsoncons::json& object, const std::string& name){
    if(!object.has_key(name)){
        logger::log(logger::LLOG::ERROR, "JsonHelper", std::string(__func__) + " Absent mandatory attribute " + name);
        throw std::runtime_error(std::string("Absent mandatory attribute. Name: ") + name);
    }
    return object[name].as<T>();
};

/*
//
*/
/*
std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
        [](unsigned char c){ return std::toupper(c); } // correct
     );
    return s;
}
*/
} // end namespace

#endif