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

std::chrono::time_point<std::chrono::system_clock> tp;
char mtime[30];

void log(const LLOG level, const std::string pattern, const std::string message){
	//struct timespec tm;
	//clock_gettime(CLOCK_REALTIME, &tm);
	//std::cout << (tm.tv_sec%126000) << ":" << (tm.tv_sec%3600) << ":" << (tm.tv_sec%60) << " | " << level << " | " << pattern << " | "<< message << std::endl;
	tp = std::chrono::system_clock::now();
	std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
	std::strftime(mtime, sizeof(mtime), "%T", std::localtime(&time_now));
	std::cout << mtime << " | " << level << " | " << pattern << " | "<< message << std::endl;
}


Logger::Logger() {
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

} /* namespace logger */
