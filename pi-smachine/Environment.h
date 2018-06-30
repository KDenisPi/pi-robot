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
    * Uptime detection
    */
    virtual const string get_uptime(){
        char buff[512];

        std::chrono::time_point<std::chrono::system_clock> _now_time; = std::chrono::system_clock::now();
        auto interval = _now_time -_start_time;
        std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(interval).count();
        int min = std::chrono::duration_cast<std::chrono::minutes>(interval).count();
        int hr = std::chrono::duration_cast<std::chrono::hours>(interval).count();

        sprinf(buff, "%u:%u:%u", hr, min, sec);
        return std::string(buff);
    }

	/*
	 * Use this mutex if you needed to share data operations
	 */
	std::recursive_mutex mutex_sm;

    std::chrono::time_point<std::chrono::system_clock> _start_time;
 };

}
#endif /* PI_SMACHINE_ENVIRONMENT_H_ */
