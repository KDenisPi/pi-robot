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
class HttpTestWebSet : public http::web::WebSettings {
public:
    HttpTestWebSet() : http::web::WebSettings(8080) {}
    ~HttpTestWebSet() {}



};

}

#endif
