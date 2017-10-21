/*
 * StateMachine.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include <unistd.h>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <pthread.h>

#include "logger.h"
#include "StateMachine.h"
#include "StateInit.h"
#include "Timers.h"

namespace smachine {

const char TAG[] = "smash";

StateMachine::StateMachine(const std::shared_ptr<StateFactory> factory, const std::shared_ptr<pirobot::PiRobot> pirobot) :
        m_factory(factory),
        m_pirobo(pirobot)
{
    m_timers = std::shared_ptr<Timers>(new Timers(this));
    m_states = std::shared_ptr<std::list<std::shared_ptr<state::State>>>(new std::list<std::shared_ptr<state::State>>);

    m_env = std::shared_ptr<Environment>(m_factory->get_environment());

    //Start timers
    m_timers->start();

    if( !start()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " State machine could not start!");
    }

    //set callback function for hardware calls
    if(pirobot.get() != nullptr){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Created callback notification function");
        pirobot->stm_notification = std::bind(&StateMachine::process_robot_notification,
                this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
}

/*
* Start State machine execution from the beginning
*/
void StateMachine::run(){
    //Add first event 
     put_event(std::shared_ptr<Event>(new Event(EVT_CHANGE_STATE, "StateInit")));
}
    

void StateMachine::state_push(const std::shared_ptr<state::State> state){
    get_states()->push_front(state);

    const std::string stack = print_state_stack();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + stack);
}

/*
 *
 */
bool StateMachine::start(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    return piutils::Threaded::start<StateMachine>(this);
}

/*
 * Temporal: Wait for processing.
 */
void StateMachine::wait(){
    piutils::Threaded::stop(false);
}

/*
 *
 */
void StateMachine::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started.");
    piutils::Threaded::stop();
}

/*
 *
 */
StateMachine::~StateMachine() {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //Stop main thread if it is not stopped yet
    this->stop();
    // Stop Equipment
    m_pirobo->stop();
    //Stop Timers support
    m_timers->stop();
    //Erase not processed states
    m_states->erase(m_states->begin(), m_states->end());

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

/*
 *
 */
const std::shared_ptr<Event> StateMachine::get_event(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    mutex_sm.lock();
    std::shared_ptr<Event> event = m_events.front();
    m_events.pop();
    mutex_sm.unlock();
    return event;
}

/*
 *
 */
void StateMachine::put_event(const std::shared_ptr<Event> event, bool force){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started:" + event->name());
    mutex_sm.lock();
    if(force){
        while(!m_events.empty())
            m_events.pop();
    }
    m_events.push(event);
    mutex_sm.unlock();
}

/*
 *
 */
void StateMachine::finish(){
    std::shared_ptr<Event> event(new Event(EVENT_TYPE::EVT_FINISH));
    put_event(event);
}

/*
 *
 */
void StateMachine::state_change(const std::string new_state){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Generate event Change state to: " + new_state);
    std::shared_ptr<Event> event(new EventChangeState(new_state));
    put_event(event);
}

/*
 *
 */
void StateMachine::state_pop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pop State");
    std::shared_ptr<Event> event(new Event(EVENT_TYPE::EVT_POP_STATE));
    put_event(event);
}

/*
 *
 */
void StateMachine::timer_start(const int timer_id, const time_t interval, const bool interval_timer){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    this->m_timers->create_timer(std::shared_ptr<Timer>(new Timer(timer_id, interval, 0, interval_timer)));
}

/*
 *
 */
void StateMachine::timer_cancel(const int timer_id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    this->m_timers->cancel_timer(timer_id);
}


/*
 *
 */
void StateMachine::worker(StateMachine* stm){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
    bool finish = false;

    for(;;){
        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker check event queue");

        /*
        TODO: Add condition variable here
        */
        while( !stm->empty() && !finish){

            auto event = stm->get_event();
            switch(event->type()){

            /*
             * Process finish event
             */
            case EVT_FINISH:
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Finish event detected.");
                stm->process_finish_event();
                finish = true;
                break;

                /*
                 * Process change state event
                 */
            case EVT_CHANGE_STATE:
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Change state event detected.");
                stm->process_change_state(event);
                break;

                /*
                 * Process pop state event
                 */
            case EVT_POP_STATE:
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Pop state event detected.");
                stm->process_pop_state();
                break;

            case EVT_TIMER:
                /*
                 * Process timer event
                 */
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Timer event detected.");
                stm->process_timer_event(event);
                break;

            case EVT_BTN_UP:
            case EVT_BTN_DOWN:
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Button event detected: " + std::to_string(event->type()));
                stm->process_event(event);
                break;
                        case EVT_ITEM_ACTIVITY:
                                stm->process_event(event);
                                break;
            case EVT_NONE:
                break;
            //default:

            }
        }

        if(finish){
            break;
        }
        /*
         *
         */
        //sleep(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
}

/*
 *
 */
void StateMachine::process_finish_event(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Process FINISH event");

    try{
        m_pirobo->stop();
    }
    catch(...){

    }
}

/*
 *
 */
bool StateMachine::process_timer_event(const std::shared_ptr<Event> event){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " timer ID: " + event->id_str());
    for (const auto& state : *(get_states())) {
        bool processed = state->OnTimer(event->id());
        if(processed){
            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " timer ID: " + event->id_str() +
                    " was processed by " + state->get_name());
            return processed;
        }
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " timer with ID: " + event->id_str() + " was not processed");
    return false;
}

/*
 *
 */
bool StateMachine::process_event(const std::shared_ptr<Event> event){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Event: " + event->name());
        for (const auto& state : *(get_states())) {
                bool processed = state->OnEvent(event);
                if(processed){
                        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Event: " + event->name() +
                                        " was processed by " + state->get_name());
                        return processed;
                }
        }

        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Event: " + event->name() + " was not processed");
        return false;
}


/*
 *
 */
void StateMachine::process_pop_state(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " pop state");
    auto state =  get_states()->front();
    try{
        /*
         *
         */
        state->OnExit();

        /*
         * Remove the first element
         */
        get_states()->pop_front();

        const std::string stack = print_state_stack();
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + stack);
    }
    catch(std::exception& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
    }
}

/*
 * Change state
 * 1. If such state is nor present in stack - just add it to the top
 * 1.2 Call OnEnter
 * 2. If such state is present already - delete all states before it
 * 3. If fabric could not construct defined state do nothing
 */
void StateMachine::process_change_state(const std::shared_ptr<Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    try{
        std::string cname = event->name();
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " state name: " + cname);

        auto newstate = (cname == "StateInit" ?
                std::shared_ptr<smachine::state::State>(new smachine::state::StateInit(dynamic_cast<StateMachineItf*>(this))):
                m_factory->get_state(cname, dynamic_cast<StateMachineItf*>(this)));
        bool new_state = true;

        for (const auto& state : *(get_states())) {
            if(state == newstate){
                new_state = false;
                break;
            }
        }
        /*
         *
         */
        if(!new_state){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " state with name present : " + cname);

            while(get_states()->front() != newstate){
                get_states()->pop_front();
            }
            newstate.reset();
        }
        else{
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " no such state, add : " + newstate->get_name());
            get_states()->push_front(newstate);

            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " call OnEntry for : " + newstate->get_name());
            auto front_state = get_states()->front();
            front_state->OnEntry();
        }

    }
    catch(std::runtime_error& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
    }
    catch(std::exception& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
    }
    catch(...){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Unknown exception");
    }

    const std::string stack = print_state_stack();
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + stack);

}

/*
 * Debug function print current stack of states
 */
const std::string StateMachine::print_state_stack() const {
    std::string result;
    for (const auto& state : *(get_states())) {
        if(result.empty()){
            result = " " + state->get_name();
        }
        else{
            result += " <-- ";
            result += state->get_name();
        }
    }

    return result;
}

/*
 *
 */
void StateMachine::process_robot_notification(int itype, std::string& name, void* data){
    logger::log(logger::LLOG::NECECCARY, __func__, " Received notification Item Type: " + std::to_string(itype) + " Name: " + name);

    try{
        if(itype == pirobot::item::ItemTypes::BUTTON || itype == pirobot::item::ItemTypes::TILT_SWITCH){
            auto  state = *(static_cast<int*>(data));
            if(state == pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED){
                std::shared_ptr<Event> btn_up(new Event(EVENT_TYPE::EVT_BTN_UP, name));
                put_event(btn_up);
            }
            else if(state == pirobot::item::BUTTON_STATE::BTN_PUSHED){
                std::shared_ptr<Event> btn_down(new Event(EVENT_TYPE::EVT_BTN_DOWN, name));
                put_event(btn_down);
            }
            else
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Unknown state for Item::Button");
        }
        else if(itype == pirobot::item::ItemTypes::AnalogMeter){
            /*
            *Analog Meter
            */
            auto values = static_cast<unsigned short*>(data);
            bool is_bright = (values[0] > values[1]);
            std::shared_ptr<Event> lmeter(new Event( (is_bright ? EVENT_TYPE::EVT_LM_HIGH : EVENT_TYPE::EVT_LM_LOW), name));
            put_event(lmeter);
        }
        else {
           std::shared_ptr<Event> item_evt(new Event(EVENT_TYPE::EVT_ITEM_ACTIVITY, name));
           put_event(item_evt);
        }
    }
    catch(...){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Unknown exception has been detected. Notification was lost");
    }

}


} /* namespace smachine */
