/*
 * Timers.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: Denis Kudia
 */
#define _POSIX_C_SOURCE 200112L //199309
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "logger.h"
#include "Timers.h"
#include "Event.h"
#include "StateMachine.h"

namespace smachine {

const char TAG[] = "timers";

Timers::Timers(StateMachine* owner) :
         m_owner(owner)
{
    sigset_t new_set;
    sigemptyset (&new_set);
    sigaddset (&new_set, SIGALRM);
    if( sigprocmask(SIG_BLOCK, &new_set, NULL) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not set signal mask.");
    }

}

Timers::~Timers() {
    // TODO Auto-generated destructor stub
}


/*
 *
 */
bool Timers::start(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    return piutils::Threaded::start<Timers>(this);
}

/*
 *
 */
void Timers::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    piutils::Threaded::stop();
    m_id_to_tm.clear();
}

/*
 *
 */
void Timers::worker(Timers* owner){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker started.");
    struct timespec timeout;
    sigset_t new_set;
    siginfo_t sig_info;

    /*
     * Allow Timer Signal
     */

    sigemptyset (&new_set);
    sigaddset (&new_set, SIGALRM);
    if( pthread_sigmask(SIG_BLOCK, &new_set, NULL) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not set thread signal mask.");
        return;
        //return (void*) 1L;
    }

    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;
    while(!owner->is_stop_signal()){
        /*
         * Wait timer signal
         */
        int res = sigtimedwait(&new_set, &sig_info, &timeout);
        if(res < 0){
            if(errno == EINTR){
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " thread interrupted by unknown signal");
                break;
            }
            else if(errno == EAGAIN){
                /*
                 * Timeout.
                 */
                //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer wait timeout ");
                continue;
            }
            else{
                break;
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " sigtimedwait failed. Error: " + std::to_string(errno));
            }
        }
        else{
            /*
             * Timer signal is received. Process it.
             */
            const int id  = sig_info._sifields._timer.si_sigval.sival_int;

            // Remove timer from map
            owner->cancel_timer(id);

            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Detected signal ID: " + std::to_string(id));
            owner->get_owner()->put_event(std::shared_ptr<Event>(new Event(EVT_TIMER, id)));
        }

    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Worker finished.");
}

/*
 *
 */
bool Timers::create_timer(const std::shared_ptr<Timer> timer){
    timer_t tid;
    struct sigevent evt;
    struct itimerspec itime;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(timer->get_id()) +
        " Sec: " + std::to_string(timer->get_time().tv_sec) + " NSec: "+ std::to_string(timer->get_time().tv_nsec));

    mutex_tm.lock();

    auto timer_old = m_id_to_tm.find(timer->get_id());
    if(timer_old != m_id_to_tm.end()){
        /*
         * Timer with such ID is present already
         */
        mutex_tm.unlock();
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer is present already.");
        return true;
    }

    evt.sigev_notify = SIGEV_SIGNAL;
    evt.sigev_signo = SIGALRM;
    evt.sigev_value.sival_int =  timer->get_id();	//Using for timer recognizing
    evt.sigev_notify_function = NULL;
    evt.sigev_notify_attributes = NULL;

    if( timer_create(CLOCK_REALTIME, &evt, &tid) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not create timer Error: " + std::to_string(errno));

        mutex_tm.unlock();
        return false;
    }

    if( timer->is_interval()){
        itime.it_interval.tv_sec = timer->get_time().tv_sec;
        itime.it_interval.tv_nsec = timer->get_time().tv_nsec;

        itime.it_value.tv_sec = 0;
        itime.it_value.tv_nsec = 0;
    }
    else{
        itime.it_interval.tv_sec = 0;
        itime.it_interval.tv_nsec = 0;

        itime.it_value.tv_sec = timer->get_time().tv_sec;
        itime.it_value.tv_nsec = timer->get_time().tv_nsec;
    }

    if(timer_settime(tid, 0, &itime, NULL) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set timer Error: " + std::to_string(errno));

        timer_delete(tid);

        mutex_tm.unlock();
        return false;
    }

    timer->set_tid(tid);
    m_id_to_tm.emplace(timer->get_id(), timer);

    mutex_tm.unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " finished.");
    return true;
}

/*
 *
 */
void Timers::cancel_timer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));

    mutex_tm.lock();

    auto timer = m_id_to_tm.find(id);
    if(timer == m_id_to_tm.end()){
        /*
         * Timer with such ID is not present
         */
        mutex_tm.unlock();
        return;
    }

    timer_t tid = timer->second->get_tid();
    if(timer_delete(tid) < 0){
        if(errno == EINVAL){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id) + " is not present already");
        }
        else
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not delete timer Error: " + std::to_string(errno));
    }

    m_id_to_tm.erase(id);

    mutex_tm.unlock();
    return;
}

/*
 *
 */
void Timers::reset_timer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));

    mutex_tm.lock();

    auto timer = m_id_to_tm.find(id);
    if(timer == m_id_to_tm.end()){
        /*
         * Timer with such ID is not present
         */
        mutex_tm.unlock();
        return;
    }

    timer_t tid = timer->second->get_tid();
    struct itimerspec itime;
    if(timer_gettime(tid, &itime) == 0){
        if(timer_settime(tid, 0, &itime, NULL) < 0)
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set timer Error: " + std::to_string(errno));
    }
    else{
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not get timer Error: " + std::to_string(errno));
    }

    mutex_tm.unlock();
    return;
}


} /* namespace smachine */
