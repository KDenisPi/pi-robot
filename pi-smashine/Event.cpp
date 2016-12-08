/*
 * Event.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "Event.h"

namespace smashine {


Event::Event(const EVENT_TYPE type):
  m_type(type), m_id(0), m_name()
{
}

Event::Event(const EVENT_TYPE type, int id):
	  m_type(type), m_id(id), m_name()
{
}

Event::Event(const EVENT_TYPE type, const std::string name):
  m_type(type), m_id(0), m_name(name)
{
}


Event::~Event() {
	// TODO Auto-generated destructor stub
}

} /* namespace smashine */
