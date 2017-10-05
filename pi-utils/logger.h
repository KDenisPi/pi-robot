/*
 * logger.h
 *
 *  Created on: Nov 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_LOGGER_H_
#define PI_LIBRARY_LOGGER_H_

#include <string>
#include <mutex>

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

	inline void log(const LLOG level, const std::string pattern, const std::string message);

private:
	std::shared_ptr<spdlog::logger> async_file;	
    std::mutex cv_m;	
	
};

} /* namespace logger */

#endif /* PI_LIBRARY_LOGGER_H_ */
