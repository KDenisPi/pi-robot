/*
 * MyEnv.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_MYENV_H_
#define PROJECT1_MYENV_H_

#include "Environment.h"

namespace project1 {

class MyEnv : public smachine::Environment {
public:
	MyEnv() : led_processed(0) {}
	virtual ~MyEnv() {}

	const int led_max = 7;
	int items[7] = {0,1,30,31,32,50,51};
	int led_processed;
};

}

#endif /* PROJECT1_MYENV_H_ */
