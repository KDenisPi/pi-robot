/**
 * @file Timers2.h
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef PI_SMACHINE_TIMERS2_H_
#define PI_SMACHINE_TIMERS2_H_

#include <time.h>
#include <signal.h>
#include <memory>
#include <map>
#include <mutex>
#include <atomic>

#include "logger.h"
#include "Timer.h"
#include "Event.h"
#include "Threaded.h"

namespace smachine {
namespace timer {

class Timers;
using TimersPtr = std::shared_ptr<smachine::timer::Timers>;

/**
 * @brief
 *
 */
struct timer_event {
    int timer_id;
    long evt_counter;
};


/**
 * @brief
 *
 */
class Timers : public piutils::Threaded
{
public:
    /**
     * @brief Construct a new Timers object
     *
     */
    Timers() {
        class_instance = this;
    }

    virtual ~Timers() {}

    std::atomic<timer_event> event_info;

    /**
     * @brief
     *
     * @param sig
     * @param si
     * @param uc
     */
    static void handler(int sig, siginfo_t* si, void* uc);

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool init();

    const bool start();
    void stop();


    virtual const bool create_timer(const struct timer_info& tm_info); // override;
    virtual const bool cancel_timer(const int id); // override;
    virtual const bool reset_timer(const int id); // override;
    virtual const bool is_timer(const int id); // override;

    static void worker(Timers* p);

    static Timers* class_instance;
    std::function<void(std::shared_ptr<smachine::Event>)> put_event = nullptr;
private:
    std::mutex mutex_tm;
    std::map<int, std::shared_ptr<Timer>> timers;
};

}
}

#endif