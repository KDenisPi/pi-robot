/*
 * logger.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: Denis Kudia
 */

#include <iostream>
#include <time.h>
#include "logger.h"

namespace logger {

void log(const LLOG level, const std::string pattern, const std::string message){
	struct timespec tm;
	clock_gettime(CLOCK_REALTIME, &tm);
	std::cout << __TIME__ << "." << (tm.tv_sec%60) << " | " << level << " | " << pattern << " | "<< message << std::endl;
}


Logger::Logger() {
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

} /* namespace logger */
