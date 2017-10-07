/*
 * MyEnv.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_MYENV_H_
#define PROJECT1_MYENV_H_

#include "Environment.h"

namespace spi_test {

class MyEnv : public smachine::Environment {
public:
	MyEnv() : m_finish(false) {}
	virtual ~MyEnv() {}

	bool m_finish;
};

}

#endif /* PROJECT1_MYENV_H_ */
