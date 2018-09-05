/*
 * smallthings.cpp
 *
 *  Simple implemnetation for file based data storage
 *
 *  Created on: Sep 4, 2018
 *      Author: Denis Kudia
 */

#include "smallthings.h"
#include <cstring>

namespace piutils {

/*
* Check if file exist and available
*/
bool chkfile(const std::string& fname){
    int res = access(fname.c_str(), F_OK);
    return (res == 0);
}

//
//
//
void get_time(std::tm& result, const bool local_time){
    std::chrono::time_point<std::chrono::system_clock> tp;
    tp = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(tp);

    //TODO Add semaphore
    std::tm* tm = (local_time ? std::localtime(&time_now) : std::gmtime(&time_now));
    std::memcpy(&result, tm, sizeof(std::tm));
}

//
//Return time string with format 'Mon, 29 Aug 2018 16:00:00 +1100'
// Used for email
//
const std::string get_time_string(const bool local_time){
  std::chrono::time_point<std::chrono::system_clock> tp;
  char mtime[128];

  tp = std::chrono::system_clock::now();
  std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
  if(std::strftime(mtime, sizeof(mtime), "%a, %d %b %Y %H:%M:%S %z", (local_time ? std::localtime(&time_now) : std::gmtime(&time_now))))
    return std::string(mtime);

  //Exception?
  return std::string();
}
}