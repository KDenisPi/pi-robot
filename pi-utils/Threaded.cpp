/*
 * Threaded.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#include "Threaded.h"

namespace piutils {

Threaded::Threaded() : m_pid(0), m_pthread(0), m_stopSignal(false)
{
}

Threaded::~Threaded() {
	// TODO Auto-generated destructor stub
}

} /* namespace pirobot */