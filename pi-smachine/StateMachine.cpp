/*
 * StateMachine.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include <unistd.h>
#include <stdexcept>
#include <exception>
#include <algorithm>

#include "logger.h"
#include "StateMachine.h"
#include "StateInit.h"
#include "Timers.h"

namespace smachine {

const char TAG[] = "smash";

StateMachine::StateMachine(const std::shared_ptr<StateFactory> factory, const std::shared_ptr<pirobot::PiRobot> pirobot) :
		m_pthread(0),
		m_factory(factory),
		m_pirobo(pirobot)
{
	m_timers = std::shared_ptr<Timers>(new Timers(this));
	m_states = std::shared_ptr<std::list<std::shared_ptr<state::State>>>(new std::list<std::shared_ptr<state::State>>);

	start();
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
	bool ret = true;
	pthread_attr_t attr;

	/*
	 *
	 */
	m_timers->start();

	if( is_stopped() ){
		state_change("StateInit");
		//this->put_event(std::shared_ptr<Event>(new Event(EVT_FINISH)), true);

		pthread_attr_init(&attr);
		int result = pthread_create(&this->m_pthread, &attr, StateMachine::worker, (void*)(this));
		if(result == 0){
			logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Thread created");
		}
		else{
			//TODO: Exception
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Thread failed Res:" + std::to_string(result));
			ret = false;
		}
	}

	return ret;
}

/*
 *
 */
void StateMachine::stop(){
	void* ret;
	int res = 0;

	if( !is_stopped() ){
		/*
		 * Generate Stop Event and push it to the events queues
		 */
		this->put_event(std::shared_ptr<Event>(new Event(EVT_FINISH)), true);
		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->m_pthread));

		res = pthread_join(this->m_pthread, &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
StateMachine::~StateMachine() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

	m_timers->stop();
	m_states->erase(m_states->begin(), m_states->end());

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

/*
 *
 */
const std::shared_ptr<Event> StateMachine::get_event(){
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
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Generate event Change state to:" + new_state);
	std::shared_ptr<Event> event(new EventChangeState(new_state));
	put_event(event);
}

/*
 *
 */
void StateMachine::state_pop(){
	std::shared_ptr<Event> event(new Event(EVENT_TYPE::EVT_POP_STATE));
	put_event(event);
}

/*
 *
 */
void StateMachine::timer_start(const int timer_id, const time_t interval, const bool interval_timer){
	this->m_timers->create_timer(std::shared_ptr<Timer>(new Timer(timer_id, interval, 0, interval_timer)));
}

/*
 *
 */
void StateMachine::timer_cancel(const int timer_id){
	this->m_timers->cancel_timer(timer_id);
}


/*
 *
 */
void* StateMachine::worker(void* p){
	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
	bool finish = false;

	StateMachine* stm = static_cast<StateMachine*>(p);

	for(;;){
		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker check event queue");

		while( !stm->empty() && !finish){

			auto event = stm->get_event();
			switch(event->type()){

			/*
			 * Process finish event
			 */
			case EVT_FINISH:
				logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Finish event detected.");
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
		sleep(1);
	}

	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
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
	try{
		std::string cname = event->name();
		logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " state name: " + cname);

		auto newstate = (cname == "StateInit" ?
				std::shared_ptr<smachine::state::State>(new smachine::state::StateInit(dynamic_cast<StateMachineItf*>(this))):
				m_factory->get_state(cname, dynamic_cast<StateMachineItf*>(this)));
		bool new_state = true;

		//throw std::runtime_error("No such state");


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


} /* namespace smachine */
