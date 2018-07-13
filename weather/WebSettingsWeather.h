/*
* WebSettingsWeather.h
*
* Web interface for Weather settings and status
*
*/
#ifndef WEATHER_WEB_SETTINGS_H
#define WEATHER_WEB_SETTINGS_H

#include "WebSettings.h"
#include "context.h"

namespace weather {
namespace web {

class WebWeather : public http::web::WebSettings {
public:
    WebWeather(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) : http::web::WebSettings(port, itf){
        mg_add_uri_handler(_server, "/index.html", WebSettings::html_pages);
        mg_add_uri_handler(_server, "/status.html", WebSettings::html_pages);
    }

    /*
    *
    */
    virtual ~WebWeather() {

    }

    /*
    *
    */
    //
    const std::string get_page_by_URI(const char *uri, const char * lg = "en") {

        if(strcmp(lg, "en") == 0){
            if(strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0){
                return "/en/index.html";
            }
            else if(strcmp(uri, "/status.html") == 0){
                return "/en/status.html";
            }
        }
        else if(strcmp(lg, "ru") == 0){
            if(strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0){
                return "/ru/index.html";
            }
            else if(strcmp(uri, "/status.html") == 0){
                return "/ru/status.html";
            }
        }

        return "";
    }

    /*
    *
    */
    virtual const std::string get_page(const struct mg_connection *conn) override {

        auto ctxt = get_context<weather::Context>();
        std::string page_name = get_page_by_URI(conn->uri, ctxt->get_language().c_str());

        //try to load page content
        std::string page = piutils::webutils::WebUtils::load_page(ctxt->get_web_root() + page_name);

        std::map<std::string, std::string> values;
        values["<!--{NetInfo}-->"] =  ctxt->ip4_address;
        values["<!--{Uptime}-->"] = ctxt->get_uptime();

        const std::string _new_page = piutils::webutils::WebUtils::replace_values(page, values);
        return _new_page;
    }
};


}//namespace web
}//namespace weather

#endif
