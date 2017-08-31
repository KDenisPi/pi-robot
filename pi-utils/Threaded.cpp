/*
 * Threaded.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#include "Threaded.h"

namespace piutils {

Threaded::Threaded() : m_stopSignal(false), m_loop_delay(100)
{
}

Threaded::~Threaded() {
    // TODO Auto-generated destructor stub
}

} /* namespace pirobot */
