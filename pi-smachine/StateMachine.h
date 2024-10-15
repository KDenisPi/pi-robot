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
class StateFactory;

class StateMachine : public StateMachineItf, public piutils::Threaded {
public:
    StateMachine(const std::shared_ptr<StateFactory> factory, const std::shared_ptr<pirobot::PiRobot> pirobot);

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
    // Generate finish signal
    virtual void finish() override;
    virtual void state_change(const std::string& new_state) override;
    void state_change_to_first();
	virtual const std::string get_first_state() override;
    virtual void state_pop() override;

	virtual bool create_timer(const struct timer::timer_info& tm_info) override;
	virtual bool cancel_timer(const int id) override;
	virtual bool reset_timer(const int id) override;
	virtual bool is_timer(const int id) override;

    virtual std::shared_ptr<pirobot::PiRobot> get_robot() override {return m_pirobot;}
    virtual std::shared_ptr<Environment> get_env() override {return m_env; }

    /*
    * Start State machine execution from the beginning
    */
    void run();

    bool process_timer_event(const std::shared_ptr<Event>& event);
    bool process_event(const std::shared_ptr<Event>& event);
    void process_pop_state(const std::shared_ptr<Event>& event);
    void process_change_state(const std::shared_ptr<Event>& event);
    void process_finish_event();


    /*
     * Temporal: Wait for processing
     */
    void wait();

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
private:
    //
    bool start();
    //
    void stop();

    //
    bool empty() const {
        return m_events.empty();
    }

    inline std::shared_ptr<std::list<std::shared_ptr<state::State>>> get_states() const {return m_states;}

    void state_push(const std::shared_ptr<state::State> state);

    std::mutex mutex_sm;
    std::queue<std::shared_ptr<Event>> m_events;
    std::shared_ptr<std::list<std::shared_ptr<state::State>>> m_states;

    /*
     * Timer support object
     */
    std::shared_ptr<smachine::timer::Timers2> m_timers;

    // Hardware configuration
    std::shared_ptr<pirobot::PiRobot> m_pirobot;
    // State factory
    std::shared_ptr<StateFactory> m_factory;
    //Environment
    std::shared_ptr<Environment> m_env;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINE_H_ */
