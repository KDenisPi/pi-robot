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
#include <cstring>
#include <unistd.h>

namespace piutils {

static void get_time(std::tm& result, const bool local_time = false);
static bool chkfile(const std::string& fname);

/*
* Check if file exist and available
*/
static bool chkfile(const std::string& fname){
    int res = access(fname.c_str(), F_OK);
    return (res == 0);
}

//
//
//
static void get_time(std::tm& result, const bool local_time){
    std::chrono::time_point<std::chrono::system_clock> tp;
    tp = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(tp);

    //TODO Add semaphore
    std::tm* tm = (local_time ? std::localtime(&time_now) : std::gmtime(&time_now));
    std::memcpy(&result, tm, sizeof(std::tm));
}



}
#endif