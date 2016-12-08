/*
 * Event.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_EVENT_H_
#define PI_SMASHINE_EVENT_H_

#include <memory>

namespace smashine {

enum EVENT_TYPE {
   EVT_NONE         = 0,  //Do nothing
   EVT_CHANGE_STATE = 1,  //Change state
   EVT_POP_STATE    = 2,  //Move to previous state in stack
   EVT_TIMER        = 3,  //Timer
   EVT_FINISH       = 4
};   

class Event {
public:
	Event(const EVENT_TYPE type);
	Event(const EVENT_TYPE type, int id);
	Event(const EVENT_TYPE type, const std::string name);

	virtual ~Event();

    inline const EVENT_TYPE type() const {return m_type;}
    inline const int id() const {return m_id;}
    inline const std::string id_str() const {return std::to_string(m_id);}
    inline const std::string name() const {return m_name;}

private:
       EVENT_TYPE m_type;
       int m_id;
       std::string m_name;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_EVENT_H_ */
