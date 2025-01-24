/*
 * StateMachine.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATEMACHINE_H_
#define PI_SMACHINE_STATEMACHINE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <list>
#include <pthread.h>

#include "Timers2.h"
#include "StateMachineItf.h"
#include "PiRobot.h"
#include "StateFactory.h"
#include "State.h"
#include "EventChangeState.h"
#include "Event.h"
#include "Threaded.h"
#include "MqttClient.h"

namespace smachine {

class Timers2;

class StateMachine : public StateMachineItf, public piutils::Threaded {
public:
    StateMachine();

    virtual ~StateMachine();

    virtual const std::string clname() const override {
        return std::string("StateMachine");
    }

    /*
    * Get next event to the event's queue
    */
    const std::shared_ptr<Event> get_event();

    /*
     * Add a new event to the queue
     *
     * set force to true if need to clear queue and push event
     */
    void put_event(const std::shared_ptr<Event>& event, bool force = false);

	virtual void add_event(const std::shared_ptr<Event>& event) override {
        this->put_event(event, false);
    }

    /*
     *
     */
    /**
     * @brief Generate finish signal
     *
     */
    virtual void finish() override{
        put_event(std::make_shared<Event>(EVENT_TYPE::EVT_FINISH, "Finish"));
    }

    /**
     * @brief
     *
     * @param new_state
     */
    virtual void state_change(const std::string& new_state) override{
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + " Generate event Change state to: " + new_state);
        put_event(std::make_shared<EventChangeState>(new_state));
    }

    /**
     * @brief
     *
     */
    void state_change_to_first(){
        state_change(getfirststate()); //get_factory()->get_first_state());
    }

    /**
     * @brief
     *
     */
    virtual void state_pop() override{
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + " Pop State");
        put_event(std::make_shared<Event>(EVENT_TYPE::EVT_POP_STATE, "PopState"));
    }

    /*
    * Start State machine execution from the beginning
    */
    void run(){
        //Add first event
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + " Started");
        put_event(std::make_shared<Event>(EVT_CHANGE_STATE, "StateInit"));
    }

    void state_push(const std::shared_ptr<state::State> state){
        get_states()->push_front(state);

        const std::string stack = print_state_stack();
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + stack);
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool init();

    /**
     * @brief Temporal: Wait for processing
     *
     */
    void wait(){
        piutils::Threaded::wait();
    }

    /**
     * @brief
     *
     * @param tm_info
     * @return true
     * @return false
     */
    virtual bool create_timer(const struct timer::timer_info& tm_info) override{
        return m_timers->create_timer(tm_info);
    }

    /**
     * @brief
     *
     * @param id
     * @return true
     * @return false
     */
    virtual bool cancel_timer(const int id) override{
        return m_timers->cancel_timer(id);
    }

    /**
     * @brief
     *
     * @param id
     * @return true
     * @return false
     */
    virtual bool reset_timer(const int id) override{
        return m_timers->reset_timer(id);
    }

    /**
     * @brief
     *
     * @param id
     * @return true
     * @return false
     */
    virtual bool is_timer(const int id) override{
        return m_timers->is_timer(id);
    }

    std::function<const std::shared_ptr<pirobot::PiRobot>()> get_robot = nullptr;
    //std::function<void(std::shared_ptr<E>)> get_env = nullptr;
    //std::function<void(std::shared_ptr<F>)> get_factory = nullptr;

    std::function<std::string()> getfirststate = nullptr;
    std::function<bool(const std::string&)>configure_environment = nullptr;
    std::function<const std::shared_ptr<smachine::state::State>(const std::string&)> get_state = nullptr;

    bool process_timer_event(const std::shared_ptr<Event>& event);
    bool process_event(const std::shared_ptr<Event>& event);
    void process_pop_state(const std::shared_ptr<Event>& event);
    void process_change_state(const std::shared_ptr<Event>& event);
    void process_finish_event();


    /*
     * Debug function print current stack of states
     */
    const std::string print_state_stack() const;

    /*
     *
     */
    void process_robot_notification(int itype, std::string& name, void* data);

    /**
     * @brief
     *
     * @param p
     */
    static void worker(StateMachine* p);

    /**
     * @brief
     *
     */
    static StateMachine* class_instance;
    static StateMachineItf* itf;
private:
    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool start(){
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + " Started");
        return piutils::Threaded::start<StateMachine>(this);
    }

    /**
     * @brief
     *
     */
    void stop(){
        logger::log(logger::LLOG::DEBUG, "smash", std::string(__func__) + " Started.");
        piutils::Threaded::stop();
    }

    //
    bool empty() const {
        return m_events.empty();
    }

    inline std::shared_ptr<std::list<std::shared_ptr<state::State>>> get_states() const {return m_states;}

    std::mutex mutex_sm;
    std::queue<std::shared_ptr<Event>> m_events;
    std::shared_ptr<std::list<std::shared_ptr<state::State>>> m_states;

    /*
     * Timer support object
     */
    std::shared_ptr<smachine::timer::Timers2> m_timers;
};

using sm = smachine::StateMachine;

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINE_H_ */
