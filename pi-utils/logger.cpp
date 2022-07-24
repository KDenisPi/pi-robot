/*
 * logger.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: Denis Kudia
 */

#include <iostream>
#include <time.h>
#include <chrono>
#include <ctime>
#include <memory>
#include <functional>
#include <utility>

#include "Threaded.h"
#include "CircularBuffer.h"

#include "logger.h"
#include "spdlog/sinks/daily_file_sink.h"


namespace logger {

std::shared_ptr<Logger> plog;

/*
std::chrono::time_point<std::chrono::system_clock> tp;
char mtime[30];
*/
/*
    char mtime[30];
    std::chrono::time_point<std::chrono::system_clock> tp;
    tp = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
    std::strftime(mtime, sizeof(mtime), "%T", std::localtime(&time_now));
    ////std::cout << mtime << " | " << level << " | " << pattern << " | "<< message << std::endl;
*/

/*
*
*/
Logger::Logger(const std::string& filename, const LLOG level) : m_flush(false), _level(level){

    ////std::cout <<  "--- Logger::Logger---" << std::endl;

    log = spdlog::create<spdlog::sinks::daily_file_sink_mt>("pi-robot", filename, 11, 59);
    log->set_level(spdlog::level::debug);
    log->set_pattern("%H:%M:%S.%e %z|%t|%L|%v");
    //log->set_pattern("%v");

    m_buff = std::make_shared<log_type>(_q_size);
    piutils::Threaded::start<Logger>(this);
}

Logger::~Logger() {
    ////std::cout <<  "--- Logger::~Logger---" << std::endl;

    set_flush();
    piutils::Threaded::stop();
    log->flush();
}

void Logger::finish() {
    ////std::cout <<  "--- Logger::finish---" << std::endl;

    log->flush();
    spdlog::shutdown();
}



/*
* Mail logging function
*/
void Logger::llog(const logger::LLOG level, const std::string& pattern, const std::string& message){

    //
    // Update logger log level configuration
    //
    if( is_update_conf() ){
        ////std::cout <<  "--- Logger::llog is_update_conf ---" << std::endl;
        update_configuration();
    }

    if(level > _level){ //ignore levels higher than defined
        ////std::cout <<  "--- Logger::llog ignore by level---" << std::endl;
        return;
    }

    //////std::cout <<  "---> " << pattern << " " << message << std::endl;
    log_message_type msg = std::make_pair(level, std::make_pair(pattern,message));
    //write_log(msg);
    //const std::string msg = pattern + " " + message;
    ////std::cout <<  msg << std::endl;
    //log->debug("{}", msg);

    m_buff->put(std::move(msg));
    cv.notify_one();
}

/*
*
*/
void Logger::write_log(const log_message_type& logm) const{
    auto logm_ = logm.second;

    if(logm.first == LLOG::INFO)
        log->info("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::DEBUG)
        log->debug("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::NECECCARY)
        log->warn("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::ERROR)
        log->error("{0} {1}", logm_.first, logm_.second);
}

/*
*
*/
void Logger::worker(Logger* owner){
    //////std::cout << "Logger worked started" << std::endl;
    auto fn = [owner]{return (owner->is_stop_signal() || owner->data_present());};

    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn);
        }

        while( (!owner->is_stop_signal()||owner->is_flush()) && owner->data_present()){
            auto logm = owner->get();
            owner->write_log(logm);
        }

        if(owner->is_flush())
            break;
    }
    //////std::cout << "Logger worked finished" << std::endl;
}

/*
*
*/
void log_init(const std::string& filename){
    if(!plog){
        ////std::cout <<  "--- Logger::log_init---" << std::endl;
        plog = std::make_shared<Logger>(filename);
    }
}

/*
*
*/
void log(const LLOG level, const std::string& pattern, const std::string& message){
    if(!plog){
        ////std::cout <<  "--- Logger::log Ignore---" << std::endl;
        return;
    }
    if(!plog->is_flush()){
        plog->llog(level, pattern, message);
    }
    else{
        ////std::cout <<  "--- Logger::log Flush Ignore---" << std::endl;
    }
}

void release(){
    ////std::cout <<  "--- Logger::release---" << std::endl;
    if(plog){
        ////std::cout <<  "--- Logger::release delete--- Count: " << plog.use_count() << std::endl;
        plog.reset();
        ////std::cout <<  "--- Logger::release delete--- Count: " << plog.use_count() << std::endl;
    }
}

void set_level(const LLOG level){
  if(plog){
    plog->set_level(level);
  }
}

void set_update_conf(){
  if(plog){
    plog->set_update_conf();
  }

}

void finish(){
    if(plog){
        plog->finish();
    }
}



} /* namespace logger */
