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

namespace logger {

std::shared_ptr<Logger> plog;
Logger* p_plog;

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
    std::cout << mtime << " | " << level << " | " << pattern << " | "<< message << std::endl;
*/

/*
*
*/
Logger::Logger(const std::string& filename, const LLOG level) : m_flush(false), _level(level){
    async_file = spdlog::daily_logger_st("async_file_logger", filename);
    async_file->set_level(spdlog::level::debug);
    async_file->set_pattern("%H:%M:%S.%e %z|%t|%L|%v");

    m_buff = std::shared_ptr<log_type>(new log_type(_q_size));
    piutils::Threaded::start<Logger>(this);
}

Logger::~Logger() {
    set_flush();
    piutils::Threaded::stop();
    async_file->flush();
}

/*
* Mail logging function
*/
void Logger::llog(const logger::LLOG level, const std::string& pattern, const std::string& message){

    //
    // Update logger log level configuration
    //
    if( is_update_conf() ){
        update_configuration();
    }

    if(level > _level)//ignore levels higher than defined
      return;

    log_message_type msg = std::make_pair(level, std::make_pair(pattern,message));
    m_buff->put(std::move(msg));
    cv.notify_one();
}

/*
*
*/
void Logger::write_log(const log_message_type& logm) const{
    auto logm_ = logm.second;

    if(logm.first == LLOG::INFO)
        async_file->info("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::DEBUG)
        async_file->debug("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::NECECCARY)
        async_file->warn("{0} {1}", logm_.first, logm_.second);
    else if(logm.first == LLOG::ERROR)
        async_file->error("{0} {1}", logm_.first, logm_.second);
}

/*
*
*/
void Logger::worker(Logger* owner){
    //std::cout << "Logger worked started" << std::endl;
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
    //std::cout << "Logger worked finished" << std::endl;
}

/*
*
*/
void log_init(const std::string& filename){
    if(!plog){
        p_plog = new Logger(filename);
        plog = std::shared_ptr<Logger>(p_plog);
    }
}

/*
*
*/
void log(const LLOG level, const std::string& pattern, const std::string& message){
    if(!plog){
        p_plog = new Logger();
        plog = std::shared_ptr<Logger>(p_plog);
    }
    if(!plog->is_flush()){
        plog->llog(level, pattern, message);
    }
}

void release(){
  if(plog){
     delete plog.get();
  }
}

void set_level(const LLOG level){
  if(!plog){
      p_plog = new Logger();
      plog = std::shared_ptr<Logger>(p_plog);
  }

  plog->set_level(level);
}

void set_update_conf(){
  if(!plog){
      p_plog = new Logger();
      plog = std::shared_ptr<Logger>(p_plog);
  }

  plog->set_update_conf();
}


} /* namespace logger */
