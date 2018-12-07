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

void log(const LLOG level, const std::string& pattern, const std::string& message);
void log_init(const std::string& filename);
void release();
void set_level(const LLOG level);

using log_message = std::pair<std::string, std::string>;
using log_message_type = std::pair<logger::LLOG, log_message>;
using log_type = piutils::circbuff::CircularBuffer<log_message_type>;

class Logger : public piutils::Threaded{
public:
    Logger(const std::string& filename = "/var/log/pi-robot/async_log");
    virtual ~Logger();

    static void worker(Logger* p);

    void llog(const LLOG level, const std::string& pattern, const std::string& message);

    const bool data_present() const {
        return !m_buff->is_empty();
    }

    const log_message_type& get(){
        return m_buff->get();
    }

    void write_log(const log_message_type& message) const;

    void set_flush(){
        m_flush = true;
    }
    const bool is_flush() const {
        return m_flush;
    }

    void set_level(const LLOG clevel){
      _level = clevel;
    }

    static logger::LLOG type_by_string(const char* dbgtype) {
        if(strcmp(dbgtype, "INFO") == 0) return logger::LLOG::INFO;
        else if(strcmp(dbgtype, "DEBUG") == 0) return  logger::LLOG::DEBUG;
        else if(strcmp(dbgtype, "ERROR") == 0) return  logger::LLOG::ERROR;
        else if(strcmp(dbgtype, "NECECCARY") == 0) return  logger::LLOG::NECECCARY;

        return logger::LLOG::INFO;
    }

private:
    std::shared_ptr<spdlog::logger> async_file;
    std::mutex cv_m;

    std::shared_ptr<log_type> m_buff;
    bool m_flush;
    LLOG _level;

    size_t _q_size = 2048; //queue size must be power of 2

};

//extern Logger plog;
extern std::shared_ptr<Logger> plog;

} /* namespace logger */

#endif /* PI_LIBRARY_LOGGER_H_ */
