/*
 * Timers.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */
#define _POSIX_C_SOURCE 200112L //199309
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "logger.h"
#include "Timers.h"

namespace smashine {

const char TAG[] = "timers";

Timers::Timers() :
		m_pthread(0), m_stop(false), m_pid(0)
{
	sigset_t new_set;
	sigemptyset (&new_set);
	sigaddset (&new_set, SIGALRM);
	if( sigprocmask(SIG_BLOCK, &new_set, NULL) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not set signal mask.");
	}

}

Timers::~Timers() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
void Timers::set_stop_signal(const bool state){
	mutex_tm.lock();
	m_stop = state;
	mutex_tm.unlock();
}

bool Timers::is_stop_signal()
{
	mutex_tm.lock();
	bool stop = m_stop;
	mutex_tm.unlock();
	return stop;
}

/*
 *
 */
bool Timers::start(){
	bool ret = true;
	pthread_attr_t attr;

	if( is_stopped() ){
		set_stop_signal(false);

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

		set_stop_signal(true);

		res = pthread_join(this->m_pthread, &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
		else
			m_pthread = 0;
	}

	m_id_to_tm.clear();
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
void* Timers::worker(void* p){
	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
	struct timespec timeout;
	sigset_t new_set;
	siginfo_t sig_info;
	Timers* owner = static_cast<Timers*>(p);

	/*
	 * Allow Timer Signal
	 */

	sigemptyset (&new_set);
	sigaddset (&new_set, SIGALRM);
	if( pthread_sigmask(SIG_BLOCK, &new_set, NULL) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not set thread signal mask.");
		return (void*) 1L;
	}

	/*
	 * Save PID for signal receiving
	 */
	owner->set_pid(syscall(SYS_gettid));
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " PID: " + std::to_string(owner->get_pid()));

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
			const int id  = sig_info._sifields._timer.si_sigval.sival_int;

			logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Detected signal ID: " + std::to_string(id));

		}

	}

	logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}

/*
 *
 */
bool Timers::create_timer(const std::shared_ptr<Timer> timer){
	timer_t tid;
	struct sigevent evt;
	struct itimerspec itime;

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(timer->get_id()) +
		" Sec: " + std::to_string(timer->get_time().tv_sec) + " NSec: "+ std::to_string(timer->get_time().tv_nsec));

	mutex_tm.lock();

	auto timer_old = m_id_to_tm.find(timer->get_id());
	if(timer_old != m_id_to_tm.end()){
		/*
		 * Timer with such ID is present already
		 */
		mutex_tm.unlock();
		return true;
	}

	evt.sigev_notify = SIGEV_SIGNAL;
	evt.sigev_signo = SIGALRM;
	evt.sigev_value.sival_int =  timer->get_id();	//Using for timer recognizing
	evt.sigev_notify_function = NULL;
	evt.sigev_notify_attributes = NULL;

	if( timer_create(CLOCK_REALTIME, &evt, &tid) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not create timer Error: " + std::to_string(errno));

		mutex_tm.unlock();
		return false;
	}

	if( timer->is_interval()){
		itime.it_interval.tv_sec = timer->get_time().tv_sec;
		itime.it_interval.tv_nsec = timer->get_time().tv_nsec;

		itime.it_value.tv_sec = 0;
		itime.it_value.tv_nsec = 0;
	}
	else{
		itime.it_interval.tv_sec = 0;
		itime.it_interval.tv_nsec = 0;

		itime.it_value.tv_sec = timer->get_time().tv_sec;
		itime.it_value.tv_nsec = timer->get_time().tv_nsec;
	}

	if(timer_settime(tid, 0, &itime, NULL) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set timer Error: " + std::to_string(errno));

		timer_delete(tid);

		mutex_tm.unlock();
		return false;
	}

	timer->set_tid(tid);
	m_id_to_tm.emplace(timer->get_id(), timer);

	mutex_tm.unlock();
	return true;
}

/*
 *
 */
void Timers::cancel_timer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));

	mutex_tm.lock();

	auto timer = m_id_to_tm.find(id);
	if(timer == m_id_to_tm.end()){
		/*
		 * Timer with such ID is not present
		 */
		mutex_tm.unlock();
		return;
	}

	timer_t tid = timer->second->get_tid();
	if(timer_delete(tid) < 0){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not delete timer Error: " + std::to_string(errno));
	}

	m_id_to_tm.erase(id);

	mutex_tm.unlock();
	return;
}

/*
 *
 */
void Timers::reset_timer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));

	mutex_tm.lock();

	auto timer = m_id_to_tm.find(id);
	if(timer == m_id_to_tm.end()){
		/*
		 * Timer with such ID is not present
		 */
		mutex_tm.unlock();
		return;
	}

	timer_t tid = timer->second->get_tid();
	struct itimerspec itime;
	if(timer_gettime(tid, &itime) == 0){
		if(timer_settime(tid, 0, &itime, NULL) < 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set timer Error: " + std::to_string(errno));
	}
	else{
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not get timer Error: " + std::to_string(errno));
	}

	mutex_tm.unlock();
	return;
}


} /* namespace smashine */
