/*
 * logger.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: denis
 */

#include <iostream>
#include "logger.h"

namespace logger {

void log(const LLOG level, const std::string pattern, const std::string message){
	std::cout << __TIME__ << " | " << level << " | " << pattern << " | "<< message << std::endl;
}


Logger::Logger() {
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

} /* namespace logger */
