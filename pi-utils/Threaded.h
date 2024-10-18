/*
 * Threaded.h
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_THREADED_H_
#define PI_LIBRARY_THREADED_H_

#include <iostream>
#include <stdlib.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <sstream>
#include <atomic>


namespace piutils {

const char TAG_TR[] = "thread";

/*
*
*/
class Threaded {
public:
    Threaded();
    virtual ~Threaded();

    inline const std::thread::id get_thread_id() const {
        return m_thread.get_id();
    }

    virtual const std::string clname() const {
        return std::string("Threaded");
    }

    const std::string get_thread_id_str(){
        if(m_thread_id_str.empty()){
            std::ostringstream ss;
            ss << m_thread.get_id();
            m_thread_id_str =  ss.str();
        }
        return m_thread_id_str;
    }

    /*
    *
    */
    inline bool is_stopped(){
        bool joinable = m_thread.joinable();
        return !joinable;
    }

    inline void wait(){
        if( !is_stopped() ){
            this->m_thread.join();
        }
    }

    /*
    *
    */
    template<class T>
    bool start(T* owner){
        bool ret = true;

        if( is_stopped() ){
            m_thread = std::thread(T::worker, owner);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return ret;
    }

    /*
    *
    */
    void stop(const bool set_stop=true);

    /*
    *
    */
    inline const bool is_stop_signal(){
        return m_stopSignal.load();
    }

    /*
    *
    */
    inline void set_stop_signal(const bool signal = true){
        std::lock_guard<std::mutex> lk(cv_m);
        m_stopSignal = signal;
        cv.notify_one();
    }

    virtual unsigned int const get_loop_delay() const {
        return m_loop_delay;
    } //thread loop delay in ms

    void set_loop_delay(const unsigned int loop_delay){
        m_loop_delay = loop_delay;
    }

private:
    std::thread m_thread;
    std::atomic_bool m_stopSignal;
    std::string m_thread_id_str;
    unsigned int m_loop_delay;

public:
    std::condition_variable cv;
    std::mutex cv_m;
};

} /* namespace piutils */

#endif /* PI_LIBRARY_THREADED_H_ */
