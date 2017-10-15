/*
 * MyEnv.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_MYENV_H_
#define PROJECT1_MYENV_H_

#include <chrono>

#include "Environment.h"

namespace project1 {

class MyEnv : public smachine::Environment {
public:
	MyEnv() : m_finish(false) {}
	virtual ~MyEnv() {}

	bool m_finish;

	#define LM_SENSOR_0 0
	#define LM_SENSOR_1 1
	
	/*
		Time when Light Sensiors detected 
	*/
	std::chrono::time_point<std::chrono::system_clock> tm_epoch;
	std::chrono::time_point<std::chrono::system_clock> lm_time[2];

	const bool is_sensor_not_set(const int idx) const {
		return (tm_epoch == lm_time[idx]);
	}

	void set_lm_time(const int idx){
		lm_time[idx] = std::chrono::system_clock::now();
	}
};

}

#endif /* PROJECT1_MYENV_H_ */
