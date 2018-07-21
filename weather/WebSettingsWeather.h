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
        mg_add_uri_handler(_server, "/data", WebWeather::data_files);
    }

    /*
    *
    */
    virtual ~WebWeather() {

    }

    /*
    *
    */
   virtual void initialize(const uint16_t port) override{
        _server = mg_create_server(this);
        mg_add_uri_handler(_server, "/", WebSettings::html_pages);
        mg_add_uri_handler(_server, "/data", WebWeather::data_files);
   }


    /*
    * Detect page location
    */
    const std::pair<std::string, std::string> get_page_by_URI(const char *uri) {

        auto ctxt = get_context<weather::Context>();
        std::string lang = ctxt->get_language();
        std::string page_path = ctxt->get_web_root();

        if(std::strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0 || strcmp(uri, "/default.html") == 0){
            if(lang == "en")
                return std::make_pair(std::string(http::web::mime_html), page_path + "/en/default.html");
            else if(lang == "ru")
                return std::make_pair(std::string(http::web::mime_html), page_path + "/ru/default.html");
        }
        else if(std::strcmp(uri, "/status.html") == 0){
            if(lang == "en")
                return std::make_pair(std::string(http::web::mime_html), page_path + "/en/status.html");
            else if(lang == "ru")
                return std::make_pair(std::string(http::web::mime_html), page_path + "/ru/status.html");
        }
        else if(std::strstr(uri, "metricsgraphics.js") != NULL){
            return std::make_pair(std::string(http::web::mime_js), page_path + "/metricsgraphics.js");
        }
        else if(std::strstr(uri, "metricsgraphics.css") != NULL){
            return std::make_pair(std::string(http::web::mime_plain), page_path + "/metricsgraphics.css");
        }

        return std::make_pair(std::string(), std::string());
    }

    /*
    *
    */
    static int data_files(struct mg_connection *conn) {
        auto owner = static_cast<WebWeather*>(conn->server_param);
        auto ctxt = owner->get_context<weather::Context>();

        if(std::strstr(conn->uri, "json") != NULL){
            std::string json_path = ctxt->get_json_data();
            std::string full_name = json_path + conn->uri;
            //
            //check if file exist
            //
            if(piutils::chkfile(full_name)){
                std::string page = piutils::webutils::WebUtils::load_page(full_name);
                if(!page.empty()){
                    mg_send_header(conn, "Content-Type:", "application/json");
                    mg_send_data(conn, page.c_str(), page.size());
                    return 0;
                }
            }
        }

        //check loaded size
        mg_send_header(conn, "Content-Type:", "text/html; charset=utf-8");
        mg_send_status(conn, 404);
        mg_printf_data(conn, "Page not found!<br> Requested URI is [%s], query string is [%s]",
                conn->uri,
                conn->query_string == NULL ? "(none)" : conn->query_string);

        return 0;
    }


    /*
    *
    */
    virtual const std::pair<std::string, std::string> get_page(const struct mg_connection *conn) override {
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__));

        auto page_name = get_page_by_URI(conn->uri);

        if(page_name.second.empty()){
            return page_name;
        }

        //
        //Check file here
        //

        //try to load page content
        std::string page = piutils::webutils::WebUtils::load_page(page_name.second);

        if(page_name.second.find("default") != std::string::npos || page_name.second.find("status") != std::string::npos ){
            auto ctxt = get_context<weather::Context>();

            std::map<std::string, std::string> values;
            values["<!--{NetInfo}-->"] =  ctxt->ip4_address;
            values["<!--{Uptime}-->"] = ctxt->get_uptime();

            logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " IP: " + ctxt->ip4_address + " UpTime: " + ctxt->get_uptime());

            /*
            * Add another patterns depends on page
            *
            * I do not know Is it right place for it or not
            */
            if(page_name.second.find("default") != std::string::npos){
                Measurement data = ctxt->data;
                std::string lang = ctxt->get_language();

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
            return std::make_pair(page_name.first, _new_page);
        }

        return std::make_pair("", "");
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
