/*
 * Timers.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */
#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "logger.h"
#include "Timers.h"

namespace smashine {

const char TAG[] = "smash";

Timers::Timers() :
		m_pthread(0), m_stop(false), m_pid(0)
{
	// TODO Auto-generated constructor stub

}

Timers::~Timers() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
bool Timers::start(){
	bool ret = true;
	pthread_attr_t attr;

	if( is_stopped() ){

		m_stop = false;

		pthread_attr_init(&attr);
		int result = pthread_create(&this->m_pthread, &attr, Timers::worker, (void*)(this));
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
void Timers::stop(){
	void* ret;
	int res = 0;

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Thread: ") + std::to_string(this->m_pthread));

	if( !is_stopped() ){
		res = pthread_join(this->m_pthread, &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
		else
			m_pthread = 0;
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
void* Timers::worker(void* p){
	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
	struct timespec timeout;
	sigset_t new_set, old_set;
	siginfo_t sig_info;
	Timers* owner = static_cast<Timers*>(p);

	/*
	 * Allow Timer Signal
	 */
	sigemptyset (&new_set);
	sigaddset (&new_set, SIGALRM);
	if( pthread_sigmask(SIG_BLOCK, &new_set, &old_set) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not create thread.");
		return (void*) 1L;
	}

	/*
	 * Save PID for signal receiving
	 */
	owner->set_pid(syscall(SYS_gettid));

	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	while(!owner->is_stop_signal()){
		/*
		 * Wait timer signal
		 */
		int res = sigtimedwait(&new_set, &sig_info, &timeout);
		if(res < 0){
			if(errno == EINTR){
				logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " thread interrupted by unknown signal");
				break;
			}
			else if(errno == EAGAIN){
				/*
				 * Timeout.
				 */
				continue;
			}
			else{
				break;
				logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " sigtimedwait failed. Error: " + std::to_string(errno));
			}
		}
		else{
			/*
			 * Timer signal is received. Process it.
			 */
			const int tid  = sig_info._sifields._timer.si_sigval.sival_int;

			logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Detected signal ID: " + std::to_string(tid));

		}

	}

	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}

/*
 *
 */
bool Timers::create_timer(const uint16_t id){
	timer_t tid;
	struct sigevent evt;

	evt.sigev_notify = SIGEV_THREAD_ID;
	evt._sigev_un._tid = get_pid();
	evt.sigev_signo = SIGALRM;
	evt.sigev_value.sival_int =  id;	//Using for timer recognizing
	evt.sigev_notify_function = nullptr;
	evt.sigev_notify_attributes = nullptr;

	if( timer_create(CLOCK_REALTIME, &evt, &tid) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not create timer Error: " + std::to_string(errno));
		return false;
	}

	return true;
}

/*
 *
 */
void Timers::cancel_timer(const uint16_t id){

}

/*
 *
 */
void Timers::reset_timer(const uint16_t id){

}


} /* namespace smashine */
