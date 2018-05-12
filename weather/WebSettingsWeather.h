/*
* WebSettingsWeather.h
*
* Web interface for Weather settings and status
*
*/
#ifndef WEATHER_WEB_SETTINGS_H
#define WEATHER_WEB_SETTINGS_H

#include "WebSettings.h"

namespace weather {
namespace web {

class WebWeather : public http::web::WebSettings {
public:
    WebWeather(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) : http::web::WebSettings(port, itf){

    }

    virtual ~WebWeather() {}

    virtual const std::string get_page(const struct mg_connection *conn) override {

        //try to load page content
        std::string page = piutils::webutils::WebUtils::load_page(get_index_page());
        const std::string ip_info =  prepare_ip_list();
        const std::string new_page = piutils::webutils::WebUtils::replace_value(page, "{NetInfo}", ip_info);

        return new_page;
    }
};


}//namespace web
}//namespace weather

#endif
