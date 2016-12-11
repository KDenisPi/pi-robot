/*
 * Timers.h
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */

#ifndef PI_SMACHINE_TIMERS_H_
#define PI_SMACHINE_TIMERS_H_

#include <pthread.h>
#include <map>
#include <mutex>
#include <memory>

#include "TimersItf.h"
#include "Timer.h"
//#include "StateMachine.h"

namespace smachine {

class StateMachine;

class Timers : public TimersItf {
public:
	Timers(StateMachine* owner);
	virtual ~Timers();

	/*
	 *
	 */
	virtual bool create_timer(const std::shared_ptr<Timer> timer) override;
	virtual void cancel_timer(const int id) override;
	virtual void reset_timer(const int id) override;

	bool is_stop_signal();

	inline void set_pid(pid_t pid) {m_pid = pid;}
	inline const pid_t get_pid() { return m_pid;}

	bool start();
	void stop();
	inline StateMachine* get_owner() const { return m_owner;}
private:

	void set_stop_signal(const bool state=true);

	inline bool is_stopped() const { return (m_pthread == 0);}

	bool m_stop;

	static void* worker(void* p);
	pthread_t m_pthread;
	pid_t m_pid;

	std::recursive_mutex mutex_tm;

	std::map<int, std::shared_ptr<Timer>> m_id_to_tm;
	StateMachine* m_owner;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_TIMERS_H_ */
