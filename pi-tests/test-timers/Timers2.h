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

#include "logger.h"
#include "Timer.h"

namespace smachine {
namespace timer {

class Timers;
using TimersPtr = std::shared_ptr<smachine::timer::Timers>;

/**
 * @brief
 *
 */
class Timers
{
public:
    Timers() {}
    virtual ~Timers() {}

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
    const bool start();

};

}
}

#endif