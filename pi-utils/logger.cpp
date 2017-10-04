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
	size_t q_size = 4096; //queue size must be power of 2
	//spdlog::set_async_mode(q_size);
	//spdlog::set_pattern("%H:%M:%S %z|%t|%v");
	async_file = spdlog::daily_logger_st("async_file_logger", "/home/denis/logs/async_log");
	async_file->set_level(spdlog::level::debug);
	//async_file->flush_on(spdlog::level::err);s
	async_file->set_pattern("%H:%M:%S %z|%t|%v");
	
}

} /* namespace logger */
