/*
 * Timers.h
 *
 *  Created on: Nov 27, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_TIMERS_H_
#define PI_SMACHINE_TIMERS_H_

#include <pthread.h>
#include <map>
#include <mutex>
#include <memory>

#include "TimersItf.h"
#include "Timer.h"
#include "Threaded.h"

namespace smachine {

class StateMachine;

class Timers : public TimersItf, public piutils::Threaded {
public:
	Timers(StateMachine* owner);
	virtual ~Timers();

	/*
	 *
	 */
	virtual bool create_timer(const std::shared_ptr<Timer> timer) override;
	virtual void cancel_timer(const int id) override;
	virtual void reset_timer(const int id) override;


	bool start();
	void stop();
	inline StateMachine* get_owner() const { return m_owner;}
	static void* worker(void* p);
private:
	std::recursive_mutex mutex_tm;

	std::map<int, std::shared_ptr<Timer>> m_id_to_tm;
	StateMachine* m_owner;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_TIMERS_H_ */
