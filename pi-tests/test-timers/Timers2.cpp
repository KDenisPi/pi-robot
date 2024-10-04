/**
 * @file Timers.cpp
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <string>
#include "Timers2.h"

namespace smachine {
namespace timer {

const char TAG[] = "timers";

/**
 * @brief
 *
 * @param sig
 * @param si
 * @param uc
 */
void Timers::handler(int sig, siginfo_t* si, void* uc){
    const int id = si->si_value.sival_int;

    printf("Tiemer ID: %d", id);
    logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Tier ID:" + std::to_string(id));
}


/**
 * @brief
 *
 * @return true
 * @return false
 */
const bool Timers::start(){
    struct sigaction sa;
    struct sigevent sev;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Timers::handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(TIMER_SIG, &sa, NULL) == -1){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set handler Error: " + std::to_string(errno));
        return false;
    }

    return true;
}

}//timer
}//smachine