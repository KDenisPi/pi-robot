/*
 * Event.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_EVENT_H_
#define PI_SMACHINE_EVENT_H_

#include <memory>

namespace smachine {

enum EVENT_TYPE {
   EVT_NONE         = 0,  	//Do nothing
   EVT_CHANGE_STATE = 1,  	//Change state
   EVT_POP_STATE    = 2,  	//Move to previous state in stack
   EVT_TIMER        = 3,  	//Timer
   EVT_BTN_UP	    = 4,  	//Button UP (Released)
   EVT_BTN_DOWN	    = 5,	//Button Down (pressed)
   EVT_ITEM_ACTIVITY = 6,
   EVT_LM_HIGH      = 7,
   EVT_LM_LOW       = 8,
   EVT_LM_VALUE     = 9,
   EVT_FINISH       = 10    //End of work
};

struct EventData{
    unsigned short ushort_[2];
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
    inline const bool is_event(const std::string ename) const {return (this->name().compare(ename) == 0);}
    inline const struct EventData& data() const {return m_data;}
    inline const std::string to_string() const {return name() + " ID: " + id_str();}

private:
       EVENT_TYPE m_type;
       int m_id;
       std::string m_name;
public:
       struct EventData m_data;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_EVENT_H_ */
