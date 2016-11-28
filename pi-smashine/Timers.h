/*
 * Timers.h
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_TIMERS_H_
#define PI_SMASHINE_TIMERS_H_

#include <pthread.h>
#include "TimersItf.h"

namespace smashine {

class Timers : public TimersItf {
public:
	Timers();
	virtual ~Timers();

	/*
	 *
	 */
	virtual bool create_timer(const uint16_t id) override;
	virtual void cancel_timer(const uint16_t id) override;
	virtual void reset_timer(const uint16_t id) override;

	bool is_stop_signal(){return m_stop;}

	void set_pid(pid_t pid) { m_pid = pid;}
	const pid_t get_pid() { return m_pid;}

private:
	bool start();
	void stop();

	bool is_stopped() const { return (m_pthread == 0);}

	bool m_stop;
	static void* worker(void* p);
	pthread_t m_pthread;
	pid_t m_pid;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_TIMERS_H_ */
