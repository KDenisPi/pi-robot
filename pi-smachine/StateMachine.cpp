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
#include "timers.h"
#include "StateMachine.h"
#include "StateInit.h"
#include "Timers.h"

namespace smachine {

const char TAG[] = "smash";

StateMachine::StateMachine(const std::shared_ptr<StateFactory> factory,
    const std::shared_ptr<pirobot::PiRobot> pirobot) :
        m_factory(factory),
        m_pirobot(pirobot)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    m_timers = std::make_shared<Timers>(this);
    m_states = std::make_shared<std::list<std::shared_ptr<state::State>>>();

    /*
    * Create project environment and load value for parameters
    */
    m_env = m_factory->get_environment();
    bool ctxt_init = m_env->configure(m_factory->get_configuration());
    if(!ctxt_init){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not load environment configuration");
		finish();
    }

    //Start timers
    m_timers->start();

    if( !start()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " State machine could not start!");
        //TODO: throw exception here?
    }

    //set callback function for hardware calls
    if(pirobot){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Created callback notification function");
        pirobot->stm_notification = std::bind(&StateMachine::process_robot_notification, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

/*
* Start State machine execution from the beginning
*/
void StateMachine::run(){
    //Add first event
     logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
     put_event(std::make_shared<Event>(EVT_CHANGE_STATE, "StateInit"));
}

const std::string StateMachine::get_first_state(){
    return m_factory->get_first_state();
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
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Stopping PI Robot");
    m_pirobot->stop();

    //Stop Timers support
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Stopping Timers");
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

    std::lock_guard<std::mutex> lock(mutex_sm);
    std::shared_ptr<Event> event = m_events.front();
    m_events.pop();
    return event;
}

/*
 *
 */
void StateMachine::put_event(const std::shared_ptr<Event>& event, bool force){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Event:" + event->name() + " ID: " + std::to_string(event->id()));

    std::cout <<  "put_event " << event->name() <<  std::endl;

    {
        std::lock_guard<std::mutex> lock(mutex_sm);
        std::cout <<  "put_event lock " << event->name() << std::endl;
        if(force){
            std::queue<std::shared_ptr<Event>> events_empty;
            m_events.swap(events_empty);
        }
        m_events.push(std::move(event));
    }

    std::cout <<  "put_event notify " << event->name() << std::endl;
    cv.notify_one();
}

/*
 *
 */
void StateMachine::finish(){
    put_event(std::make_shared<Event>(EVENT_TYPE::EVT_FINISH, "Finish"));
}

/*
 *
 */
void StateMachine::state_change(const std::string& new_state){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Generate event Change state to: " + new_state);
    put_event(std::make_shared<EventChangeState>(new_state));
}

/*
 *
 */
void StateMachine::state_pop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pop State");
    put_event(std::make_shared<Event>(EVENT_TYPE::EVT_POP_STATE, "PopState"));
}

/*
 *
 */
bool StateMachine::timer_start(const int timer_id, const time_t interval, const bool interval_timer){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    return this->m_timers->create_timer(std::make_shared<Timer>(timer_id, interval, 0, interval_timer));
}

/*
 *
 */
bool StateMachine::timer_cancel(const int timer_id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    return this->m_timers->cancel_timer(timer_id);
}

//check if timer is running
bool StateMachine::timer_check(const int timer_id){
    return m_timers->is_timer(timer_id);
}


/*
 *
 */
void StateMachine::worker(StateMachine* stm){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
    bool finish = false;

    auto fn = [stm]{return (stm->is_stop_signal() || !stm->empty());};

    for(;;){
        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker check event queue");

        {
            std::unique_lock<std::mutex> lk(stm->cv_m);
            stm->cv.wait(lk, fn);
        }

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
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Finish event detected.");
                stm->process_finish_event();
                finish = true;
                break;

                /*
                 * Process change state event
                 */
            case EVT_CHANGE_STATE:
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Change state event detected.");
                stm->process_change_state(event);
                break;

                /*
                 * Process pop state event
                 */
            case EVT_POP_STATE:
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Pop state event detected.");
                stm->process_pop_state(event);
                break;

            case EVT_TIMER:
                /*
                 * Process timer event
                 */
                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer event detected.");
                stm->process_timer_event(event);
                break;

            case EVT_BTN_UP:
            case EVT_BTN_DOWN:
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Button event detected: " + std::to_string(event->type()));
                stm->process_event(event);
                break;
            case EVT_ITEM_ACTIVITY:
                stm->process_event(event);
                break;
            case EVT_NONE:
                break;
            default:
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Event detected: " + std::to_string(event->type()) +
                    " Name: " + event->name() + " ID: " + event->id_str());
                stm->process_event(event);
            }
        }

        if(finish){
            break;
        }
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
}

/*
 *
 */
void StateMachine::process_finish_event(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Process FINISH event");

    try{
        m_pirobot->stop();
    }
    catch(...){

    }
}

/*
 *
 */
bool StateMachine::process_timer_event(const std::shared_ptr<Event>& event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + event->id_str());
    for (const auto& state : *(get_states())) {
        bool processed = state->OnTimer(event->id());
        if(processed){
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " timer ID: " + event->id_str() + " was processed by " + state->get_name());
            return processed;
        }
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " timer with ID: " + event->id_str() + " was not processed");
    return false;
}

/*
 *
 */
bool StateMachine::process_event(const std::shared_ptr<Event>& event){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Event: " + event->name());
        for (const auto& state : *(get_states())) {
                bool processed = state->OnEvent(event);
                if(processed){
                        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Event: " + event->name() + " was processed by " + state->get_name());
                        return processed;
                }
        }

        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Event: " + event->name() + " was not processed");
        return false;
}


/*
 *
 */
void StateMachine::process_pop_state(const std::shared_ptr<Event>& event){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " pop state");
    auto state =  get_states()->front();
    const std::string substate_name = state->get_name();
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

        auto front_state = get_states()->front();
        front_state->OnSubstateExit(substate_name);
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
void StateMachine::process_change_state(const std::shared_ptr<Event>& event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    try{
        const std::string cname = event->name();
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " state name: " + cname);

        bool new_state = true;
        auto newstate = (cname == "StateInit") ? std::make_shared<smachine::state::StateInit>(dynamic_cast<StateMachineItf*>(this)) : m_factory->get_state(cname, dynamic_cast<StateMachineItf*>(this));
/*
        auto newstate = (cname == "StateInit" ?
            std::make_shared<smachine::state::StateInit>(std::shared_ptr<StateMachineItf>(dynamic_cast<StateMachineItf*>(this))) :
            m_factory->get_state(cname, std::shared_ptr<StateMachineItf>(dynamic_cast<StateMachineItf*>(this))));
*/
        if( !newstate ){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Not supported state: " + cname);
            return;
        }


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

            auto front_state = get_states()->front();
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Front state. call OnEntry for : " + front_state->get_name());
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
                put_event(std::make_shared<Event>(EVENT_TYPE::EVT_BTN_UP, name));
            }
            else if(state == pirobot::item::BUTTON_STATE::BTN_PUSHED){
                put_event(std::make_shared<Event>(EVENT_TYPE::EVT_BTN_DOWN, name));
            }
            else
                logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Unknown state for Item::Button");
        }
        else if(itype == pirobot::item::ItemTypes::AnalogLightMeter){
            /*
            *Analog Meter
            */
            auto values = static_cast<unsigned short*>(data);
            if(values[0] != values[1]){
              bool is_bright = (values[0] < values[1]);
              put_event(std::make_shared<Event>( (is_bright ? EVENT_TYPE::EVT_LM_HIGH : EVENT_TYPE::EVT_LM_LOW), name));
            }
        }
        else {
           put_event(std::make_shared<Event>(EVENT_TYPE::EVT_ITEM_ACTIVITY, name));
        }
    }
    catch(...){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Unknown exception has been detected. Notification was lost");
    }

}


} /* namespace smachine */
