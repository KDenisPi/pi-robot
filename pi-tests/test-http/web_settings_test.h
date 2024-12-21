/**
 * @file web_settings_test.h
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef HTTP_TEST_WEB_SETTINGS_H
#define HTTP_TEST_WEB_SETTINGS_H

#include "WebSettings.h"
#include "Environment.h"

namespace http_test {

class Context : public smachine::Environment {
public:
    Context() : smachine::Environment() {
        refresh();
    }

    const std::string hello() const {
        return std::string("Hello!");
    }

    void refresh(){
        piutils::netinfo::NetInfo netInfo;
        ip4_address = netInfo.detect_ip_address_by_type(piutils::netinfo::IpType::IP_V4);
        ip6_address = netInfo.detect_ip_address_by_type(piutils::netinfo::IpType::IP_V6);
    }

    std::string ip4_address;
    std::string ip6_address;
};

class HttpTestWebSet : public http::web::WebSettings {
public:
    HttpTestWebSet() : http::web::WebSettings(8080) {}
    ~HttpTestWebSet() {}

    virtual const std::string html_post_processing(const std::string& page_name, const std::string& html) override;

};

} //namespace

#endif
