/*
 * Environment.h
 *
 *  Created on: Dec 14, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_ENVIRONMENT_H_
#define PI_SMACHINE_ENVIRONMENT_H_

#include <chrono>
#include <mutex>
#include <string>
#include <cstdio>

#include "logger.h"
#include "JsonHelper.h"
#include "MqttServerInfo.h"

namespace smachine {

/*
 * Use Environment class for saving your own data
 */
class Environment{
public:
    Environment() {
        _start_time = std::chrono::system_clock::now();
    }
    virtual ~Environment() {}

    /*
    * Start time in Local/UTC
    */
   virtual const std::string get_start_time(const bool utc = false) const {
        char buff[128];
        std::time_t time_now = std::chrono::system_clock::to_time_t(_start_time);
        std::strftime(buff, sizeof(buff), "%T %Y-%m-%d", (utc ? std::gmtime(&time_now) : std::localtime(&time_now)));

        return std::string(buff);
   }

    /*
    * Uptime detection
    */
    virtual const std::string get_uptime(){
        char buff[128];

        std::chrono::time_point<std::chrono::system_clock> _now_time = std::chrono::system_clock::now();
        auto interval = _now_time -_start_time;
        int sec = std::chrono::duration_cast<std::chrono::seconds>(interval).count();
        int min = std::chrono::duration_cast<std::chrono::minutes>(interval).count();
        int hr = std::chrono::duration_cast<std::chrono::hours>(interval).count();

        min = min % 60;
        sec = sec % 60;

        if(hr < 24)
            sprintf(buff, "%u hr %.2u min %.2u sec", hr, min, sec);
        else{
            int days = hr / 24;
            hr = hr % 24;
            sprintf(buff, "%u days %u hr %.2u min %.2u sec", days, hr, min, sec);
        }
        return std::string(buff);
    }

    /*
    *
    */
    bool configure(const std::string& conf){
        if(conf.empty()){
            logger::log(logger::LLOG::ERROR, "Env", std::string(__func__) + " No Robot environment configuration");
            return false;
        }

        logger::log(logger::LLOG::NECECCARY, "Env", std::string(__func__) + " Loading Robot environment configuration ...");
        std::ifstream ijson(conf);
        try{
            logger::log(logger::LLOG::NECECCARY, "Env", std::string(__func__) + " Parse JSON from: " + conf);

            jsoncons::json conf = jsoncons::json::parse(ijson);

            /*
            * Load general application parameters
            */
            if(conf.contains("context"))
            {
                const jsoncons::json& json_context  =  conf["context"];
                _log_path = jsonhelper::get_attr<std::string>(json_context, "log_path", "/var/log/pi-robot");
                _log_name = jsonhelper::get_attr<std::string>(json_context, "log_name", "async_file_logger");
                _log_file = jsonhelper::get_attr<std::string>(json_context, "log_file", "async_log");
                _web_pages = jsonhelper::get_attr<std::string>(json_context, "web_pages", "../");
            }

            /*
            * Load email parameters
            */
            if(conf.contains("email"))
            {
                const jsoncons::json& json_context  =  conf["email"];
                _email_server = jsonhelper::get_attr<std::string>(json_context, "server", "");
                _email_cert = jsonhelper::get_attr<std::string>(json_context, "certificate", "");
                _email_user = jsonhelper::get_attr<std::string>(json_context, "user", "");
                _email_password = jsonhelper::get_attr<std::string>(json_context, "password", "");
                _email_from = jsonhelper::get_attr<std::string>(json_context, "from", "");
                _email_to = jsonhelper::get_attr<std::string>(json_context, "to", "");
            }

            /*
            * Load mqtt configuration
            */
            if(conf.contains("mqtt"))
            {
                const jsoncons::json& json_mqtt  =  conf["mqtt"];
                //host name is mandatory
                auto mqtt_enable = jsonhelper::get_attr<bool>(json_mqtt, "enable", false);
                auto host = jsonhelper::get_attr_mandatory<std::string>(json_mqtt, "host");
                auto clientid = jsonhelper::get_attr<std::string>(json_mqtt, "client_id", "");

                auto keep_alive = jsonhelper::get_attr<int>(json_mqtt, "keep_alive", 10);
                auto qos = jsonhelper::get_attr<int>(json_mqtt, "qos", 0);

                auto tls = jsonhelper::get_attr<bool>(json_mqtt, "tls", false);
                //default port for TLS is 8883 otherwise 1883
                auto port = jsonhelper::get_attr<int>(json_mqtt, "port", (tls ? 8883 : 1883));

                _mqtt_conf.set_host(host);
                _mqtt_conf.set_port(port);
                _mqtt_conf.set_clientid(clientid);
                _mqtt_conf.set_tls(tls);
                _mqtt_conf.set_enable( host.empty() ? false : mqtt_enable);

                if(tls){
                    auto tls_ca_file = jsonhelper::get_attr_mandatory<std::string>(json_mqtt, "tls_ca_file");
                    auto tls_insecure = jsonhelper::get_attr<bool>(json_mqtt, "tls_insecure", false);
                    auto tls_version = jsonhelper::get_attr<std::string>(json_mqtt, "tls_version", "tlsv1.2");

                    _mqtt_conf.set_cafile(tls_ca_file);
                    _mqtt_conf.set_tls_insecure(tls_insecure);
                    _mqtt_conf.set_tls_version(tls_version);
                }
            }
        }
        catch(jsoncons::ser_error& perr){
            logger::log(logger::LLOG::ERROR, "Env", std::string(__func__) + " Invalid configuration " +
                perr.what() + " Line: " + std::to_string(perr.line()) + " Column: " + std::to_string(perr.column()));

            return false;
        }

        logger::log(logger::LLOG::NECECCARY, "Env", std::string(__func__) + " Robot environment configuration loaded");
        return true;
    }

    /*
     * Use this mutex if you needed to share data operations
     */
    std::recursive_mutex mutex_sm;
    std::chrono::time_point<std::chrono::system_clock> _start_time;

    /*
    *  Configuration data, paths etc
    */

    //file based storage folder
    std::string _fstor_path = "/var/data/pi-robot/data";
    bool _fstor_local_time = true;

    //log location
    std::string _log_path = "/var/log/pi-robot";
    std::string _log_name = "async_file_logger";
    std::string _log_file = "async_log";

    /*
    * Email paramaters
    */
    std::string _email_server;
    std::string _email_cert;
    std::string _email_user;
    std::string _email_password;
    std::string _email_from;
    std::string _email_to;

    /*
    * Web pages location
    */
    std::string _web_pages = "../web";
    std::string _data_path = "/var/data/pi-robot";

    const std::string get_web_root() const {
        return _web_pages;
    }

    const std::string get_json_data() const {
        return _data_path + "/json";
    }

    const std::string get_csv_data() const {
        return _data_path;
    }

    //mqtt client configuration file
    mqtt::MqttServerInfo _mqtt_conf;
 };

}
#endif /* PI_SMACHINE_ENVIRONMENT_H_ */
