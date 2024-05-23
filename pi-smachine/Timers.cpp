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
#include "timers.h"
#include "Timers.h"
#include "Event.h"
#include "StateMachine.h"

namespace smachine {

const char TAG[] = "timers";

Timers::Timers(StateMachine* owner) :
         m_owner(owner)
{

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
    siginfo_t sig_info;
    sigset_t new_set, org_set;
    int res = 0;

    /*
     * Allow Timer Signal
     */

    res = sigemptyset (&new_set);
    res = sigaddset (&new_set, SIGALRM);
    res = pthread_sigmask(SIG_UNBLOCK, &new_set, &org_set);
    if(res < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not set signal mask.");
        return;
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Unblock SIGALRM signal");

    while(!owner->is_stop_signal()){
        /*
         * Wait timer signal
         */
        std::memset(&sig_info, 0, sizeof(siginfo_t));
        timeout.tv_sec = 1;
        timeout.tv_nsec = 0;

        res = sigtimedwait(&new_set, &sig_info, &timeout);
        if(res < 0){
            if(errno == EINTR){
                ////std::cout <<  "----------- timer ERROR ----" << std::endl;
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " thread interrupted by unknown signal");
                break;
            }
            else if(errno == EAGAIN){
                /*
                 * Timeout.
                 */
                ////std::cout <<  "----------- timer TIMEOUT ----" << std::endl;
                //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer wait timeout ");
                continue;
            }
            else{
                //std::cout <<  "----------- timer ERROR 1 ---- " << errno << std::endl;
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " sigtimedwait failed. Error: " + std::to_string(errno));
                break;
            }
        }
        else{
            /*
             * Timer signal is received. Process it.
             */
            const int id  = sig_info._sifields._timer.si_sigval.sival_int;
            const int tid = sig_info.si_timerid;
            const int ovrr = sig_info.si_overrun;

            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " ID: " + std::to_string(id) + " TID: " + std::to_string(tid) + " Ovr: " + std::to_string(ovrr));

            ////std::cout <<  "----------- timer SIGNAL ----" << std::endl;
            // Remove timer from map
            owner->cancel_timer(id);

            ////std::cout <<  "----------- timer SIGNAL 1 ----" << std::endl;
            logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Detected signal ID: " + std::to_string(id));
            owner->get_owner()->put_event(std::make_shared<Event>(EVT_TIMER, id));
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

    ////std::cout <<  "----------- create timer start ----" << std::endl;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(timer->get_id()) +
        " Sec: " + std::to_string(timer->get_time().tv_sec) + " NSec: "+ std::to_string(timer->get_time().tv_nsec));

    std::lock_guard<std::mutex> lock(mutex_tm);

    auto timer_old = m_id_to_tm.find(timer->get_id());
    if(timer_old != m_id_to_tm.end()){
        //Timer with such ID is present already
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
        return false;
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer ID: " + std::to_string(timer->get_id()) + " SID: " + std::to_string((uintmax_t)tid));

    timer->set_tid(tid); //save system timer ID
    m_id_to_tm.emplace(timer->get_id(), timer);

    ////std::cout <<  "----------- create timer end ----" << std::endl;

    return true;
}

/*
 *
 */
bool Timers::cancel_timer(const int id, const bool del_timer /*= true*/){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));
    int err = 0;
    timer_t tid = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_tm);
        auto timer = m_id_to_tm.find(id);
        if(timer == m_id_to_tm.end()){
            //Timer with such ID is not present
            return true;
        }

        tid = timer->second->get_tid();
        if(del_timer){
            if(timer_delete(tid) < 0){
                err == errno;
            }
        }
        m_id_to_tm.erase(id);
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer ID: " + std::to_string(id) + " SID: " + std::to_string((uintmax_t)tid));

    if(err != 0){
        if(err == EINVAL){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id) + " is not present already");
        }
        else
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not delete timer Error: " + std::to_string(err));
    }

    return (err==0 || err == EINVAL);
}

/*
 *
 */
bool Timers::reset_timer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(id));
    int err = 0;

    std::lock_guard<std::mutex> lock(mutex_tm);
    auto timer = m_id_to_tm.find(id);
    if(timer != m_id_to_tm.end()){
        timer_t tid = timer->second->get_tid();
        struct itimerspec itime;
        if(timer_gettime(tid, &itime) == 0){
            if(timer_settime(tid, 0, &itime, NULL) < 0)
                err == errno;
                logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not set timer Error: " + std::to_string(errno));
        }
        else{
            err == errno;
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not get timer Error: " + std::to_string(errno));
        }
    }
    return (err==0);
}


} /* namespace smachine */
