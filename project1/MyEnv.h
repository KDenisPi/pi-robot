/*
 * MyEnv.h
 *
 *  Created on: Dec 8, 2016
 *      Author: denis
 */

#ifndef PROJECT1_MYENV_H_
#define PROJECT1_MYENV_H_

#include "Environment.h"

namespace project1 {

class MyEnv : public smachine::Environment {
public:
	MyEnv() : led_processed(0) {}
	virtual ~MyEnv() {}

	const int led_max = 8;
	int led_processed;
};

}

#endif /* PROJECT1_MYENV_H_ */
