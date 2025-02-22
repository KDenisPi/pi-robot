/**
 * @file Timers2.cpp
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <string>
#include <tuple>
#include "Timers2.h"

namespace smachine {
namespace timer {

const char TAG[] = "Timers2";

Timers2* Timers2::class_instance = nullptr;

/**
 * @brief
 *
 * @param sig
 * @param si
 * @param uc
 */
void Timers2::handler(int sig, siginfo_t* si, void* uc){
    const int id = si->si_value.sival_int;

    if(class_instance->put_event){
        class_instance->put_event(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_TIMER, id));
    }
/*
    const auto cur_val = class_instance->event_info.load();
    if(cur_val.evt_counter >= 1000){
        const struct timer_event n_val = {id,0};
        class_instance->event_info.store(n_val);
    }
    else{
        const struct timer_event n_val = {id,cur_val.evt_counter+1};
        class_instance->event_info.store(n_val);
    }
*/
}


/**
 * @brief
 *
 */
void Timers2::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //piutils::Threaded::stop();
    timers.clear();
}


/**
 * @brief Initialize signal part. Should be called from class thread
 *
 * @return true
 * @return false
 */
const bool Timers2::init(){
    struct sigaction sa;
    struct sigevent sev;
    sigset_t new_set, org_set;

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__));

    /* Unblock processing for TIMER_SIG signal*/
    sigemptyset (&new_set);
    sigaddset (&new_set, TIMER_SIG);
    sigprocmask(SIG_UNBLOCK, &new_set, &org_set);
    pthread_sigmask(SIG_BLOCK, &new_set, &org_set);

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Timers2::handler;
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
const bool Timers2::create_timer(const struct timer_info& tm_info, const bool reset_if_exist){
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Timer ID:" + std::to_string(tm_info.id));

    const auto is_timer = timers.find(tm_info.id);
    if(is_timer != timers.end()){
        if(reset_if_exist){
            is_timer->second->start();
            return true;
        }
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer is present already.");
        return false;
    }

    const auto timer = smachine::timer::Timer::create(tm_info);
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
const bool Timers2::cancel_timer(const int id){
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
const bool Timers2::reset_timer(const int id){
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
const bool Timers2::is_timer(const int id){
    std::lock_guard<std::mutex> lock(mutex_tm);
    auto timer = timers.find(id);
    return (timer != timers.end());
}

const timer_id Timers2::get_timer_ids(const int id){
    std::lock_guard<std::mutex> lock(mutex_tm);
    auto timer = timers.find(id);
    if(timer != timers.end()){
        return std::make_tuple(id, timer->second->get_tid());
    }
    return std::make_tuple(-1,0);
}


}//timer
}//smachine