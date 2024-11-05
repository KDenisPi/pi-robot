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

namespace smachine {

const char TAG[] = "smash";


StateMachine* StateMachine::class_instance = nullptr;

StateMachine::StateMachine()
{
    class_instance = this;
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    ////std::cout  "StateMachine::StateMachine started" << std::endl;

    m_timers = std::make_shared<timer::Timers2>();
    m_states = std::make_shared<std::list<std::shared_ptr<state::State>>>();


    ////std::cout  "StateMachine::StateMachine finished" << std::endl;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
const bool StateMachine::init(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    m_timers->put_event = std::bind(&StateMachine::add_event, this, std::placeholders::_1);
    m_timers->init();

    /*
    * Create project environment and load value for parameters
    */
    bool ctxt_init = configure_environment(get_robot()->get_configuration());
    if(!ctxt_init){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not load environment configuration");
        ////std::cout  "StateMachine::StateMachine finish" << std::endl;
		finish();
        return false;
    }

    if( !start()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " State machine could not start!");
        return false;
    }

    //set callback function for hardware calls
    if(get_robot()){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Created callback notification function");
        get_robot()->stm_notification = std::bind(&StateMachine::process_robot_notification, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    return true;
}


/**
 * @brief Destroy the State Machine:: State Machine object
 *
 */
StateMachine::~StateMachine() {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    ////std::cout  "StateMachine::~StateMachine started" << std::endl;

    //Stop main thread if it is not stopped yet
    this->stop();
    // Stop Equipment
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Stopping PI Robot");
    get_robot()->stop();

    //Stop Timers support
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Stopping Timers");
    m_timers->stop();

    //Erase not processed states
    m_states->erase(m_states->begin(), m_states->end());

    ////std::cout  "StateMachine::~StateMachine finished" << std::endl;

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
    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Event:" + event->name() + " ID: " + std::to_string(event->id()));
    {
        std::lock_guard<std::mutex> lock(mutex_sm);
        if(force){
            std::queue<std::shared_ptr<Event>> events_empty;
            m_events.swap(events_empty);
        }
        m_events.push(std::move(event));
    }
    cv.notify_one();
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
                const std::string msg = " Event detected: " + std::to_string(event->type()) + " Name: " + event->name() + " ID: " + event->id_str();
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + msg);
                ////std::cout  msg << std::endl;

                stm->process_event(event);
            }
        }

        if(finish){
            break;
        }
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
    ////std::cout  "State Machine Worker finished." << std::endl;
}

/*
 *
 */
void StateMachine::process_finish_event(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Process FINISH event");

    try{
        get_robot()->stop();
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
        auto newstate = (cname == "StateInit") ? std::make_shared<smachine::state::StateInit>() : get_state(cname);
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
