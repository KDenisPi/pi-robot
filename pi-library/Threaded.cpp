/*
 * Threaded.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: denis
 */

#include "Threaded.h"

namespace pirobot {

Threaded::Threaded() : m_pthread(0), m_stopSignal(false)
{
}

Threaded::~Threaded() {
	// TODO Auto-generated destructor stub
}

} /* namespace pirobot */
