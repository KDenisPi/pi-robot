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

Logger plog;

std::chrono::time_point<std::chrono::system_clock> tp;
char mtime[30];

void log(const LLOG level, const std::string pattern, const std::string message){
/*
	tp = std::chrono::system_clock::now();
	std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
	std::strftime(mtime, sizeof(mtime), "%T", std::localtime(&time_now));
	std::cout << mtime << " | " << level << " | " << pattern << " | "<< message << std::endl;
*/
	plog.log(level, pattern, message);
}

Logger::Logger() {
	size_t q_size = 2048; //queue size must be power of 2
	//spdlog::set_async_mode(q_size, spdlog::async_overflow_policy::block_retry);
	async_file = spdlog::daily_logger_st("async_file_logger", "/var/log/pi-robot/async_log");
	async_file->set_level(spdlog::level::debug);
	//async_file->flush_on(spdlog::level::err);
	async_file->set_pattern("%H:%M:%S %z|%t|%L|%v");

	m_buff = std::shared_ptr<log_type>(new log_type(2048));
}

void Logger::log(const logger::LLOG level, const std::string& pattern, const std::string& message){
	log_message_type msg = std::make_pair(level, std::make_pair(pattern,message));
	m_buff->put(msg);
}

/*
*
*/
void Logger::llog(const log_message_type logm) const{
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
    auto fn = [owner]{return (owner->is_stop_signal() || owner->data_present());};
	
		while(!owner->is_stop_signal()){
			//wait until stop signal will be received or we will have steps for processing
			{
				std::unique_lock<std::mutex> lk(owner->cv_m);
				owner->cv.wait(lk, fn);
			}
	
			while(!owner->is_stop_signal() && owner->data_present()){
				auto logm = owner->get();
				owner->llog(logm);
			}
		}
		

}


} /* namespace logger */
