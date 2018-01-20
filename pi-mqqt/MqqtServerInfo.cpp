/*
 * MqqtServerInfo.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: Denis Kudia
 */

#include <string>
#include "jsoncons/json.hpp"
#include "logger.h"
#include "JsonHelper.h"

#include "MqqtServerInfo.h"

namespace mqqt {

const char TAG[] = "mqqtInfo";

MqqtServerInfo MqqtServerInfo::load(const std::string& json_file){
    MqqtServerInfo mqqt_info = MqqtServerInfo();

    std::ifstream ijson(json_file);
    try{
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Parse JSON from: " + json_file);

        jsoncons::json conf = jsoncons::json::parse(ijson);

        auto version = jsonhelper::get_attr<std::string>(conf, "version", "1.0");

        //host name is mandatory
        auto host = jsonhelper::get_attr_mandatory<std::string>(conf, "host");
        if(host.empty()){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration. Host name is mandatory parameter.");
            throw std::runtime_error(std::string("Invalid configuration. Host name is mandatory parameter."));
        }
        auto clientid = jsonhelper::get_attr<std::string>(conf, "client_id", "");

        auto keep_alive = jsonhelper::get_attr<int>(conf, "keep_alive", 10);
        auto qos = jsonhelper::get_attr<int>(conf, "qos", 0);

        auto tls = jsonhelper::get_attr<bool>(conf, "tls", false);
        //default port for TLS is 8883 otherwise 1883 
        auto port = jsonhelper::get_attr<int>(conf, "port", (tls ? 8883 : 1883));

        mqqt_info.set_host(host);
        mqqt_info.set_clientid(clientid);

        if(tls){
            auto tls_ca_file = jsonhelper::get_attr_mandatory<std::string>(conf, "tls_ca_file");
            auto tls_insecure = jsonhelper::get_attr<bool>(conf, "tls_insecure", false);
            auto tls_version = jsonhelper::get_attr<std::string>(conf, "tls_version", "tlsv1.2");
        }

    }
    catch(jsoncons::parse_error& perr){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration " + 
            perr.what() + " Line: " + std::to_string(perr.line_number()) + " Column: " + std::to_string(perr.column_number()));
    }

    return mqqt_info;
}

}//end namespace
