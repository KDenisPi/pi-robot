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
#include "Threaded.h"
#include "CircularBuffer.h"

namespace logger {

    enum LLOG {
        ERROR = 0,
        NECECCARY = 1,
        INFO = 2,
        DEBUG = 3
    };

void log(const LLOG level, const std::string pattern, const std::string message);    

typedef std::pair<std::string, std::string> log_message;
typedef std::pair<logger::LLOG, log_message> log_message_type;
typedef piutils::circbuff::CircularBuffer<log_message_type> log_type;

class Logger : public piutils::Threaded{
public:
    Logger();
    
    virtual ~Logger() {
        piutils::Threaded::stop();
        async_file->flush();
    }

    static void worker(Logger* p);

    void llog(const LLOG level, const std::string& pattern, const std::string& message);

    const bool data_present() const {
        return !m_buff->is_empty();
    }

    const log_message_type get(){
        return m_buff->get();
    }

    void write_log(const log_message_type) const;

private:
    std::shared_ptr<spdlog::logger> async_file;
    std::mutex cv_m;

    std::shared_ptr<log_type> m_buff;
};

extern Logger plog;    

} /* namespace logger */

#endif /* PI_LIBRARY_LOGGER_H_ */
