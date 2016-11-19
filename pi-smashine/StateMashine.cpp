/*
 * StateMashine.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "logger.h"
#include "StateMashine.h"


namespace smashine {

const char TAG[] = "smash";

StateMashine::StateMashine() :
		m_pthread(0)
{
	// TODO Auto-generated constructor stub

}

/*
 *
 */
bool StateMashine::start(){
	bool ret = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int result = pthread_create(&this->m_pthread, &attr, StateMashine::worker, (void*)(this));
	if(result == 0){
		logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Thread created");
	}
	else{
		//TODO: Exception
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Thread failed Res:" + std::to_string(result));
		ret = false;
	}

	return ret;
}

/*
 *
 */
void StateMashine::stop(){
	void* ret;
	int res = 0;

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->m_pthread));

	if( !is_stopped() ){

		/*
		 * Generate Stop Event and push it to the events queues
		 */


		res = pthread_join(this->m_pthread, &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
	}

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
StateMashine::~StateMashine() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
const std::shared_ptr<Event> StateMashine::get_event(){
	mutex_sm.lock();
	std::shared_ptr<Event> event = m_events.front();
	m_events.pop();
	mutex_sm.unlock();
	return event;
}

void StateMashine::put_event(const std::shared_ptr<Event> event, bool force){

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
void* StateMashine::worker(void* p){
	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker started.");

	//Button* owner = static_cast<Button*>(p);

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}



} /* namespace smashine */
