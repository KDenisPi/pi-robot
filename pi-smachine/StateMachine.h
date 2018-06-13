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

#include "StateMachineItf.h"
#include "PiRobot.h"
#include "StateFactory.h"
#include "State.h"
#include "EventChangeState.h"
#include "Event.h"
#include "Threaded.h"
#include "MqqtClient.h"

#define ADD_SIGNAL(signal)   sigset_t new_set;\
  sigemptyset (&new_set);\
  sigaddset (&new_set, signal);\
  if( sigprocmask(SIG_BLOCK, &new_set, NULL) < 0){\
     cout <<  " Could not set signal mask." << endl;\
  }


namespace smachine {

class Timers;

class StateMachine : public StateMachineItf, public piutils::Threaded {
public:
    StateMachine(const std::shared_ptr<StateFactory> factory,
        const std::shared_ptr<pirobot::PiRobot> pirobot,
        const std::shared_ptr<mqqt::MqqtItf> mqqt);
    virtual ~StateMachine();

    /*
    * Get next event to the event's queue
    */
    const std::shared_ptr<Event> get_event();

    /*
     * Add a new event to the queue
     *
     * set force to true if need to clear queue and push event
     */
    void put_event(const std::shared_ptr<Event> event, bool force = false);

    /*
     *
     */
    // Generate finish signal
    virtual void finish() override;
    virtual void state_change(const std::string& new_state) override;
	virtual const std::string get_first_state() override;

    virtual void state_pop() override;
    virtual void timer_start(const int timer_id, const time_t interval, const bool interval_timer) override;
    virtual void timer_cancel(const int timer_id) override;

    virtual std::shared_ptr<pirobot::PiRobot> get_robot() override {return m_pirobot;}
    virtual std::shared_ptr<Environment> get_env() override {return m_env; }

	/*
	* Get Data Interface
	*/
/*
	template<class T> std::shared_ptr<T> get_datainterface(){
		return std::static_pointer_cast<T>(_dataItf);
	}
*/

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

    static void worker(StateMachine* p);

private:
    //
    bool start();
    //
    void stop();

    //
    bool empty() const {return m_events.empty();}

    inline std::shared_ptr<std::list<std::shared_ptr<state::State>>> get_states() const {return m_states;}

    void state_push(const std::shared_ptr<state::State> state);

    std::recursive_mutex mutex_sm;
    std::queue<std::shared_ptr<Event>> m_events;
    std::shared_ptr<std::list<std::shared_ptr<state::State>>> m_states;

    /*
     * Timer support object
     */
    std::shared_ptr<smachine::Timers> m_timers;

    // Hardware configuration
    std::shared_ptr<pirobot::PiRobot> m_pirobot;
    // State factory
    std::shared_ptr<StateFactory> m_factory;
    //Environment
    std::shared_ptr<Environment> m_env;

    //MQQT support flag
    bool m_mqqt_active;
    std::shared_ptr<mqqt::MqqtItf> m_mqqt;
    const std::string m_topic;

    //Send State Mashine state to MQQT server. Topic is "stm"
    void report_state(const std::string& message){
        publish(m_topic, message);
    }

public:
    const bool is_mqqt() const {return m_mqqt_active;}

    virtual const mqqt::MQQT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload) override{
        if(is_mqqt())
            return m_mqqt->publish(topic, payload);

        return mqqt::MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

    virtual const mqqt::MQQT_CLIENT_ERROR publish(const std::string& topic, const int payloadsize, const void* payload) override{
        if(is_mqqt())
            return m_mqqt->publish(topic, payloadsize, payload);

        return mqqt::MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINE_H_ */
