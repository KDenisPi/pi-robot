/*
 * Threaded.h
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_THREADED_H_
#define PI_LIBRARY_THREADED_H_

#include <pthread.h>

namespace pirobot {

class Threaded {
public:
	Threaded();
	virtual ~Threaded();

	inline pthread_t get_thread() { return m_pthread;}
	inline void set_thread(const pthread_t thread) { m_pthread = thread;}
	inline bool is_stopped() { return (m_pthread == 0);}
	inline bool is_stopSignal() {  return m_stopSignal; }
	inline void set_stop_signal(const bool signal) { m_stopSignal = signal;}
private:
	pthread_t m_pthread;
	bool m_stopSignal;

};

} /* namespace pirobot */

#endif /* PI_LIBRARY_THREADED_H_ */
