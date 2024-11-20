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
#include "cJSON_wrap.h"
#include "MqttServerInfo.h"

namespace smachine {

/*
 * Use Environment class for saving your own data
 */
class Environment{
public:
    Environment() : _use_file_storage(false), _use_sql_storage(false) {
        class_instance = this;
        _start_time = std::chrono::system_clock::now();
    }
    virtual ~Environment() {}

    static Environment* class_instance;

    template<class T>
    const T* get_env() {
        return dynamic_cast<T*>(class_instance);
    }

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

        const std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(conf));
        if(!fl->is_success()){
            logger::log(logger::LLOG::ERROR, "Env", std::string(__func__) + "Could not load file " + fl->get_name() + " Error: " + std::to_string(fl->get_err()));
            return false;
        }

        logger::log(logger::LLOG::NECECCARY, "Env", std::string(__func__) + " Loading Robot environment configuration ...");

        logger::log(logger::LLOG::NECECCARY, "Env", std::string(__func__) + " Parse JSON from: " + conf);
        std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>(fl);
        if(!cjson->is_succeess()){
            logger::log(logger::LLOG::ERROR, "Env", std::string(__func__) + " Could not parse JSON: " + cjson->get_error());
            return false;
        }

        try{

            /*
            * Load general application parameters
            */
            if(cjson->contains("context"))
            {
                auto json_context  =  cjson->get_object("context");
                _log_path = cjson->get_attr_string_def(json_context, "log_path", "/var/log/pi-robot");
                _log_name = cjson->get_attr_string_def(json_context, "log_name", "async_file_logger");
                _log_file = cjson->get_attr_string_def(json_context, "log_file", "async_log");
            }

            if(cjson->contains("http"))
            {
                auto json_http  =  cjson->get_object("http");
                web_pages =cjson->get_attr_string_def(json_http, "web_pages", web_pages);
                web_port = cjson->get_attr_def<int>(json_http, "port", web_port);
            }

            /*
            * Load email parameters
            */
            if(cjson->contains("email"))
            {
                auto json_email  =  cjson->get_object("email");
                _email_server = cjson->get_attr_string_def(json_email, "server", "");
                _email_cert = cjson->get_attr_string_def(json_email, "certificate", "");
                _email_user = cjson->get_attr_string_def(json_email, "user", "");
                _email_password = cjson->get_attr_string_def(json_email, "password", "");
                _email_from = cjson->get_attr_string_def(json_email, "from", "");
                _email_to = cjson->get_attr_string_def(json_email, "to", "");
            }

            /*
            * Load MQTT configuration
            */
            if(cjson->contains("mqtt"))
            {
                auto json_mqtt  =  cjson->get_object("mqtt");
                //host name is mandatory
                auto mqtt_enable = cjson->get_attr_def<bool>(json_mqtt, "enable", false);
                auto host = cjson->get_attr_string(json_mqtt, "host");
                auto clientid = cjson->get_attr_string_def(json_mqtt, "client_id", "");

                auto keep_alive = cjson->get_attr_def<int>(json_mqtt, "keep_alive", 10);
                auto qos = cjson->get_attr_def<int>(json_mqtt, "qos", 0);

                auto tls = cjson->get_attr_def<bool>(json_mqtt, "tls", false);
                //default port for TLS is 8883 otherwise 1883
                auto port = cjson->get_attr_def<int>(json_mqtt, "port", (tls ? 8883 : 1883));

                _mqtt_conf.set_host(host);
                _mqtt_conf.set_port(port);
                _mqtt_conf.set_clientid(clientid);
                _mqtt_conf.set_tls(tls);
                _mqtt_conf.set_enable( host.empty() ? false : mqtt_enable);

                if(tls){
                    auto tls_ca_file = cjson->get_attr_string(json_mqtt, "tls_ca_file");
                    auto tls_insecure = cjson->get_attr_def<bool>(json_mqtt, "tls_insecure", false);
                    auto tls_version = cjson->get_attr_string_def(json_mqtt, "tls_version", "tlsv1.2");

                    _mqtt_conf.set_cafile(tls_ca_file);
                    _mqtt_conf.set_tls_insecure(tls_insecure);
                    _mqtt_conf.set_tls_version(tls_version);
                }
            }
            else
            {
                 _mqtt_conf.set_enable(false);
            }

            /*
            * SQL data storage. for future use
            */
            if(cjson->contains("sql"))
            {
            }
            else {
                set_use_sql_storage(false);
            }

            /*
            * File based data storage
            */
            if(cjson->contains("file"))
            {
                auto json_file  =  cjson->get_object("file");
                auto file_enable = cjson->get_attr_def<bool>(json_file, "enable", true);
                auto data_path = cjson->get_attr_string_def(json_file, "data_path", "/var/data/pi-robot/data");
                auto local_time = cjson->get_attr_def<bool>(json_file, "local_time", true);

                set_use_file_storage(file_enable);
                _fstor_path = data_path;
                _fstor_local_time = local_time;
            }
            else {
                //Use file storage by default
                set_use_file_storage(true);
            }


        }
        catch(std::runtime_error& exc){
            logger::log(logger::LLOG::ERROR, "Env", std::string(__func__) + exc.what());
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
    * HTTP Web pages root directory
    */
    std::string web_pages = "../web";
    uint16_t web_port = 8080;


    std::string _data_path = "/var/data/pi-robot";

    /**
     * @brief Get the web root object
     *
     * @return const std::string
     */
    const std::string get_web_root() const {
        return web_pages;
    }

    /**
     * @brief Get the web port object
     *
     * @return const std::uint16_t
     */
    const std::uint16_t get_web_port() const {
        return web_port;
    }

    const std::string get_json_data() const {
        return _data_path + "/json";
    }

    const std::string get_csv_data() const {
        return _data_path;
    }


    const bool use_file_storage() const {
        return _use_file_storage;
    }

    const bool use_mqtt_storage() const {
        return _mqtt_conf.is_enable();
    }

    const bool use_sql_storage() const {
        return _use_sql_storage;
    }

    void set_use_file_storage(const bool use_file_storage){
        _use_file_storage = use_file_storage;
    }

    void set_use_mqtt_storage(const bool use_mqtt_storage){
        _mqtt_conf.set_enable(use_mqtt_storage);
    }

    void set_use_sql_storage(const bool use_sql_storage){
        _use_sql_storage = use_sql_storage;
    }


    //MQTT client configuration file
    mqtt::MqttServerInfo _mqtt_conf;

    bool _use_file_storage = false;
    bool _use_sql_storage = false;

 };

}
#endif /* PI_SMACHINE_ENVIRONMENT_H_ */
