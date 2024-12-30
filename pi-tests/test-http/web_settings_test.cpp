/**
 * @file web_settings_test.cpp
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "web_settings_test.h"

namespace http_test {

//#ifndef GET_ENV
//#define GET_ENV(T) smachine::env::Environment::class_instance->get_env<T>()
//#endif

const std::string HttpTestWebSet::html_post_processing(const std::string& page_name, const std::string& html){
    if(page_name.find("default") != std::string::npos || page_name.find("status") != std::string::npos ){
        std::map<std::string, std::string> values;
/*
        auto ctxt = GET_ENV(http_test::Context);
        values["<!--{NetInfo}-->"] =  ctxt->ip4_address;
        values["<!--{Uptime}-->"] = ctxt->get_uptime();

        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " IP: " + ctxt->ip4_address + " UpTime: " + ctxt->get_uptime());
*/
        const std::string _new_page = piutils::webutils::WebUtils::replace_values(html, values);
        return _new_page;
    }

    return html;
}


}