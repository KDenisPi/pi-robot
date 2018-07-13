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
#include "logger.h"

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
    * Detect page location
    */
    const std::string get_page_by_URI(const char *uri, const char * lg = "en") {

        if(strcmp(lg, "en") == 0){
            if(strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0 || strcmp(uri, "/default.html") == 0){
                return "/en/default.html";
            }
            else if(strcmp(uri, "/status.html") == 0){
                return "/en/status.html";
            }
        }
        else if(strcmp(lg, "ru") == 0){
            if(strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0 || strcmp(uri, "/default.html") == 0){
                return "/ru/default.html";
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
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__));

        auto ctxt = get_context<weather::Context>();
        std::string lang = ctxt->get_language();
        std::string page_name = get_page_by_URI(conn->uri, lang.c_str());
        std::string page_path = ctxt->get_web_root();

        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " Page: " + page_name + " URI: " +
            (conn->uri == nullptr ? "Empty" : std::string(conn->uri)) + " Path: " + page_path);

        if(page_name.empty()){
            return page_name;
        }

        //
        //Check file here
        //

        //try to load page content
        std::string page = piutils::webutils::WebUtils::load_page(page_path + page_name);

        std::map<std::string, std::string> values;
        values["<!--{NetInfo}-->"] =  ctxt->ip4_address;
        values["<!--{Uptime}-->"] = ctxt->get_uptime();

        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " IP: " + ctxt->ip4_address + " UpTime: " + ctxt->get_uptime());

        /*
        * Add another patterns depends on page
        *
        * I do not know Is it right place for it or not
        */
        if(page_name.find("default") != std::string::npos){
            Measurement data = ctxt->data;

            values["<!--{Temperature}-->"] = std::to_string(data.temp(ctxt->show_temperature_in_celcius()));
            values["<!--{TempUnits}-->"] = (ctxt->show_temperature_in_celcius() ? " C" : " F" );
            values["<!--{Humidity}-->"] = std::to_string(data.si7021_humidity);
            values["<!--{Pressure}-->"] = std::to_string(data.bmp280_pressure);
            values["<!--{CO2}-->"] = std::to_string(data.spg30_co2);
            values["<!--{TVOC}-->"] = std::to_string(data.spg30_tvoc);

            int co2_level = ctxt->get_CO2_level();
            values["<!--{CO2_Level}-->"] = prepare_level_name(ctxt->get_level_label(co2_level, lang), co2_level);

            int tvoc_level = ctxt->get_TVOC_level();
            values["<!--{TVOC_Level}-->"] = prepare_level_name(ctxt->get_level_label(tvoc_level, lang), tvoc_level);

        }

        const std::string _new_page = piutils::webutils::WebUtils::replace_values(page, values);
        //logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + "\n" + _new_page);
        return _new_page;
    }

private:
    const std::string prepare_level_name(const std::string& level_name, const int level){
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " Level:" + std::to_string(level));

        if(level == 0)
            return "<span style=\"color:green\">" + level_name + "</span>";
        if(level == 1)
            return "<span style=\"color:green\">" + level_name + "</span>";
        if(level == 2)
            return "<span style=\"color:green\">" + level_name + "</span>";
        if(level == 3)
            return "<span style=\"color:yellow\">" + level_name + "</span>";

        return "<span style=\"color:red\">" + level_name + "</span>";
    }
};


}//namespace web
}//namespace weather

#endif
