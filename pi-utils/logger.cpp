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
}

void Logger::log(const LLOG level, const std::string pattern, const std::string message){
	std::lock_guard<std::mutex> lk(cv_m);

	if(level == LLOG::INFO)
		async_file->info("{0} {1}", pattern, message);
	else if(level == LLOG::DEBUG)
		async_file->debug("{0} {1}", pattern, message);
	else if(level == LLOG::NECECCARY)
		async_file->warn("{0} {1}", pattern, message);
	else if(level == LLOG::ERROR)
		async_file->error("{0} {1}", pattern, message);
}

} /* namespace logger */
