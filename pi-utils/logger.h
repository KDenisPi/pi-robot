/*
 * logger.h
 *
 *  Created on: Nov 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_LOGGER_H_
#define PI_LIBRARY_LOGGER_H_

#include <string>
#include "spdlog/spdlog.h"

namespace logger {

enum LLOG {
	ERROR = 0,
	NECECCARY = 1,
	INFO = 2,
	DEBUG = 3
};

void log(const LLOG level, const std::string pattern, const std::string msg);

class Logger {
public:
	Logger();
	
	virtual ~Logger() {
		async_file->flush();
	}

	void log(const LLOG level, const std::string pattern, const std::string message){
		if(level == LLOG::INFO)
			async_file->info("{0}|{1}", pattern, message);
		else if(level == LLOG::DEBUG)
			async_file->debug("{0}|{1}", pattern, message);
		else if(level == LLOG::NECECCARY)
			async_file->warn("{0}|{1}", pattern, message);
		else if(level == LLOG::ERROR)
			async_file->error("{0}|{1}", pattern, message);
	}

private:
	std::shared_ptr<spdlog::logger> async_file;	
};

} /* namespace logger */

#endif /* PI_LIBRARY_LOGGER_H_ */
