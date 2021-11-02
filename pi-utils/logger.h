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
#include <fstream>

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
void set_update_conf();

using log_message = std::pair<std::string, std::string>;
using log_message_type = std::pair<logger::LLOG, log_message>;
using log_type = piutils::circbuff::CircularBuffer<log_message_type>;

class Logger : public piutils::Threaded{
public:
    Logger(const std::string& filename = "/var/log/pi-robot/async_log", const LLOG level=LLOG::DEBUG);
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

    const bool is_update_conf() const {
        return _update_conf;
    }

    /*
    * Update logger configuration from the configuration file
    */
    void update_configuration() {

        std::ifstream istrm( _logger_conf, std::ios::in );
        if(istrm.is_open()){
            std::string line;
            while (std::getline(istrm, line))
            {
                int llevel = get_parameter_value(line, "llevel=");
                if( llevel >= LLOG::ERROR && llevel <= LLOG::DEBUG){
                    std::cout <<  "Log level changed to " << llevel << std::endl;
                    set_level((LLOG)llevel);
                }
            }

            istrm.close();
        }

        _update_conf = false;
    }

    /*
    * Get value for parameter=value
    */
    const std::string get_value(const std::string& cline, const std::string& pname){
        std::string::size_type pos = cline.find( pname );
        if( pos != std::string::npos){
            return cline.substr(pos + pname.length());
        }

        return std::string();
    }

    int get_parameter_value(const std::string& cline, const std::string& pname){
        std::string value = get_value(cline, pname);
        return std::stoi(value);
    }


    static logger::LLOG type_by_string(const char* dbgtype) {
        if(strcmp(dbgtype, "INFO") == 0) return logger::LLOG::INFO;
        else if(strcmp(dbgtype, "DEBUG") == 0) return  logger::LLOG::DEBUG;
        else if(strcmp(dbgtype, "ERROR") == 0) return  logger::LLOG::ERROR;
        else if(strcmp(dbgtype, "NECECCARY") == 0) return  logger::LLOG::NECECCARY;

        return logger::LLOG::INFO;
    }

    inline void set_update_conf() {
        _update_conf = true;
    }
private:
    std::shared_ptr<spdlog::logger> log;
    std::mutex cv_m;

    std::shared_ptr<log_type> m_buff;
    bool m_flush;
    LLOG _level;

    bool _update_conf = true;
    //File with logger configuration.
    std::string _logger_conf = "/var/log/pi-robot/logger.conf";

    size_t _q_size = 2048; //queue size must be power of 2

};

//extern Logger plog;
extern std::shared_ptr<Logger> plog;

} /* namespace logger */

#endif /* PI_LIBRARY_LOGGER_H_ */
