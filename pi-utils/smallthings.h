/*
 * smallthings.h
 *
 *  Simple implemnetation for file based data storage
 *
 *  Created on: Jun 19, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SMALLTHINGS_H_
#define PI_LIBRARY_SMALLTHINGS_H_

#include <chrono>
#include <ctime>
#include <string>
#include <unistd.h>

namespace piutils {

void get_time(std::tm& result, const bool local_time = false);
bool chkfile(const std::string& fname);
const std::string get_time_string(const bool local_time = true);
}
#endif