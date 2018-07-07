/*
 * Environment.h
 *
 *  Created on: Dec 14, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_ENVIRONMENT_H_
#define PI_SMACHINE_ENVIRONMENT_H_

#include <chrono>
#include <mutex>
#include <string>
#include <cstdio>

namespace smachine {

/*
 * Use Environment class for saving your own data
 */
class Environment{
public:
    Environment() {
        _start_time = std::chrono::system_clock::now();
    }
    virtual ~Environment() {}

    /*
    * Start time in Local/UTC
    */
   virtual const std::string get_start_time(const bool utc = false) const {
        char buff[128];
        std::time_t time_now = std::chrono::system_clock::to_time_t(_start_time);
        std::strftime(buff, sizeof(buff), "%T %Y-%m-%d", (utc ? std::gmtime(&time_now) : std::localtime(&time_now)));

        return std::string(buff);
   }

    /*
    * Uptime detection
    */
    virtual const std::string get_uptime(){
        char buff[128];

        std::chrono::time_point<std::chrono::system_clock> _now_time = std::chrono::system_clock::now();
        auto interval = _now_time -_start_time;
        int sec = std::chrono::duration_cast<std::chrono::seconds>(interval).count();
        int min = std::chrono::duration_cast<std::chrono::minutes>(interval).count();
        int hr = std::chrono::duration_cast<std::chrono::hours>(interval).count();

        min = min % 60;
        sec = sec % 60;

        sprintf(buff, "%u:%.2u:%.2u", hr, min, sec);
        return std::string(buff);
    }

    /*
     * Use this mutex if you needed to share data operations
     */
    std::recursive_mutex mutex_sm;
    std::chrono::time_point<std::chrono::system_clock> _start_time;

    /*
    *  Configuration data, paths etc
    */

    //file based storage folder
    std::string _fstor_path = "/var/data/pi-robot/data";
    bool _fstor_local_time = false;

    //log location
    std::string _log_path = "/var/log/pi-robot";
    std::string _log_name = "async_file_logger";
    std::string _log_file = "async_log";

    //MQQT client configuration file
    std::string _mqqt_conf;
    bool use_mqqt = false;

 };

}
#endif /* PI_SMACHINE_ENVIRONMENT_H_ */
