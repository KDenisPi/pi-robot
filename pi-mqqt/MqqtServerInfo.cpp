/*
 * MqqtServerInfo.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: Denis Kudia
 */

#include <string>
#include "jsoncons/json.hpp"

#include "MqqtServerInfo.h"

namespace mqqt {

MqqtServerInfo MqqtServerInfo::load(const std::string& json_file){
    MqqtServerInfo mqqt_info = MqqtServerInfo();
    return mqqt_info;
}

}
