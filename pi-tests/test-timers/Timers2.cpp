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

 Timers* Timers::class_instance = nullptr;

/**
 * @brief
 *
 * @param sig
 * @param si
 * @param uc
 */
void Timers::handler(int sig, siginfo_t* si, void* uc){
    const int id = si->si_value.sival_int;

    const auto cur_val = class_instance->event_info.load();
    if(cur_val.evt_counter >= 1000)
        class_instance->event_info = {id,0};
    else
        class_instance->event_info = {id,cur_val.evt_counter+1};
}


/**
 * @brief
 *
 * @return true
 * @return false
 */
const bool Timers::start(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    return piutils::Threaded::start<Timers>(this);
}

/**
 * @brief
 *
 */
void Timers::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    piutils::Threaded::stop();
    timers.clear();
}


/**
 * @brief Initialize signal part. Should be called from class thread
 *
 * @return true
 * @return false
 */
const bool Timers::init(){
    struct sigaction sa;
    struct sigevent sev;
    sigset_t new_set, org_set;

    /* Unblock processing for TIMER_SIG signal*/
    sigemptyset (&new_set);
    sigaddset (&new_set, TIMER_SIG);
    pthread_sigmask(SIG_UNBLOCK, &new_set, &org_set);

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Timers::handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(TIMER_SIG, &sa, NULL) == -1){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set handler Error: " + std::to_string(errno));
        return false;
    }

    return true;
}

/**
 * @brief
 *
 * @param tm_info
 * @return true
 * @return false
 */
const bool Timers::create_timer(const struct timer_info& tm_info){
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Timer ID:" + std::to_string(tm_info.id));

    auto is_timer = timers.find(tm_info.id);
    if(is_timer != timers.end()){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer is present already.");
        return true;
    }

    auto timer = smachine::timer::Timer::create(tm_info);
    if(timer){
        {
            std::lock_guard<std::mutex> lock(mutex_tm);
            timers.emplace(tm_info.id, timer);
        }
        timer->start();
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer created and run. ID: " + std::to_string(tm_info.id));
        return true;
    }

    return false;
}

/**
 * @brief
 *
 * @param id
 * @return true
 * @return false
 */
const bool Timers::cancel_timer(const int id){
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Timer ID:" + std::to_string(id));

    {
        std::lock_guard<std::mutex> lock(mutex_tm);
        auto timer = timers.find(id);
        if(timer != timers.end()){
            timer->second->stop();
            return true;
        }
    }

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No such timer.");
    return false;
}

/**
 * @brief
 *
 * @param id
 * @return true
 * @return false
 */
const bool Timers::reset_timer(const int id){
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Timer ID:" + std::to_string(id));

    {
        std::lock_guard<std::mutex> lock(mutex_tm);
        auto timer = timers.find(id);
        if(timer != timers.end()){
            timer->second->start();
            return true;
        }
    }

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " No such timer.");
    return false;
}

/**
 * @brief
 *
 * @param id
 * @return true
 * @return false
 */
const bool Timers::is_timer(const int id){
    std::lock_guard<std::mutex> lock(mutex_tm);
    auto timer = timers.find(id);
    return (timer != timers.end());
}

/**
 * @brief
 *
 * @param owner
 */
void Timers::worker(Timers* p){
    timer_event evt_data = {0,0};

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");

    p->init();

    auto fn = [p](long cnt_cur){return (cnt_cur == p->event_info.load().evt_counter) && !p->is_stop_signal();};


    for(;;){
        while(fn(evt_data.evt_counter))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        evt_data = p->event_info;

        if(p->is_stop_signal()){
            break;
        }

        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " TID: " + std::to_string(evt_data.timer_id) + " Cnt: " + std::to_string(evt_data.evt_counter));
    }
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
}

}//timer
}//smachine