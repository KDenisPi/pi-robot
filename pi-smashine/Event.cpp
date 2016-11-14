/*
 * Event.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "Event.h"

namespace smashine {


Event::Event(const EVENT_TYPE type, EventData* data):
  m_type(type),
  m_data(data)
{
	// TODO Auto-generated constructor stub

}

Event::~Event() {
	// TODO Auto-generated destructor stub
}

} /* namespace smashine */
