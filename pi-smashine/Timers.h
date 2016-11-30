/*
 * Timers.h
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_TIMERS_H_
#define PI_SMASHINE_TIMERS_H_

#include <pthread.h>
#include <map>
#include <mutex>
#include <memory>

#include "TimersItf.h"
#include "Timer.h"

namespace smashine {

class Timers : public TimersItf {
public:
	Timers();
	virtual ~Timers();

	/*
	 *
	 */
	virtual bool create_timer(const std::shared_ptr<Timer> timer) override;
	virtual void cancel_timer(const int id) override;
	virtual void reset_timer(const int id) override;

	bool is_stop_signal();

	void set_pid(pid_t pid) {m_pid = pid;}
	const pid_t get_pid() { return m_pid;}

	bool start();
	void stop();
private:

	void set_stop_signal(const bool state=true);

	bool is_stopped() const { return (m_pthread == 0);}

	bool m_stop;

	static void* worker(void* p);
	pthread_t m_pthread;
	pid_t m_pid;

	std::recursive_mutex mutex_tm;

	std::map<int, std::shared_ptr<Timer>> m_id_to_tm;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_TIMERS_H_ */
