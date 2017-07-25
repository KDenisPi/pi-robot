/*
 * Threaded.h
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_THREADED_H_
#define PI_LIBRARY_THREADED_H_

#include <pthread.h>
#include <mutex>

#include "logger.h"

namespace piutils {

const char TAG_TR[] = "thread";

class Threaded {
public:
	Threaded();
	virtual ~Threaded();

	inline const pthread_t get_thread() const { return m_pthread;}

	inline void set_thread(const pthread_t thread) { 
		mutex_.lock();
		m_pthread = thread;
		mutex_.unlock();
	}

	inline bool is_stopped() { return (m_pthread == 0);}

/*
*
*/
template<class T>
	bool start(T* owner){
		logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + " Started");
		bool ret = true;
		pthread_attr_t attr;

		if( is_stopped() ){
			set_stop_signal(false);

			pthread_attr_init(&attr);
			pthread_t pthread;

			int result = pthread_create(&pthread, &attr, T::worker, (void*)(owner));
			if(result == 0){
				set_thread(pthread);
				logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + " Thread created");
			}
			else{
				//TODO: Exception
				logger::log(logger::LLOG::ERROR, TAG_TR, std::string(__func__) + " Thread failed Res:" + std::to_string(result));
				ret = false;
			}
		}

		return ret;
	}

/*
*
*/
	void stop (const bool set_stop=true){
		void* ret;
		int res = 0;

		if(set_stop)
			set_stop_signal(true);

		logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->get_thread()));

		if( !is_stopped() ){
			res = pthread_join(this->get_thread(), &ret);
			if(res != ESRCH)
				logger::log(logger::LLOG::ERROR, TAG_TR, std::string(__func__) + " Could not join to thread Res: " + std::to_string(res));
			else{
				logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + " Finished already. Res: " + std::to_string(res));
			}
		}

		logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
	}

/*
*
*/
	inline const bool is_stop_signal(){
		mutex_.lock();
		bool stop = m_stopSignal;
		mutex_.unlock();
		return stop;
	}

/*
*
*/
	inline void set_stop_signal(const bool signal){ 
		mutex_.lock();
		m_stopSignal = signal;
		mutex_.unlock();
	}

	inline void set_pid(pid_t pid) {
		mutex_.lock();
		m_pid = pid;
		mutex_.unlock();
	}

	inline const pid_t get_pid() const { 
		return m_pid;
	}

	virtual unsigned int  const get_loopDelay() const {return 100;} //thread loop delay in ms
private:
	pthread_t m_pthread;
	pid_t m_pid;
	bool m_stopSignal;
	std::recursive_mutex mutex_;
};

} /* namespace pirobot */

#endif /* PI_LIBRARY_THREADED_H_ */
