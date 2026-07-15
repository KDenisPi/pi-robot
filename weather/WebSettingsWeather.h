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
    WebWeather(const uint16_t port) : http::web::WebSettings(port){
        logger::log(logger::LLOG::DEBUG, "WebW", std::string(__func__));

        initialize();
    }

    /*
    *
    */
    virtual ~WebWeather() {
        logger::log(logger::LLOG::DEBUG, "WebW", std::string(__func__));
    }

    /*
    *
    */
   virtual bool initialize() override {
        logger::log(logger::LLOG::DEBUG, "WebW", std::string(__func__));

        //make post ptrocessing for HTML pages
        set_flag_html_post_processing(true);
        return true;
   }


    /*
    * Detect page location
    */
    const http::web::pinfo get_page_by_URI(const std::string& uri) {
        const auto file = uri_file(uri);
        auto ctxt = GET_ENV(weather::Context);
        std::string lang = ctxt->get_language();
        std::string page_path = get_web_root();
        std::string result;

        logger::log(logger::LLOG::INFO, "WebW", std::string(__func__) + " URI:" + uri + " File: " + file + " Lng: " + lang);

        if(file.empty() || (file == "index.html") || (file == "default.html")){
            result = page_path + "/" + lang + "/default.html";
        }
        else if((file == "status.html") || (file == "graph.html") || (file == "files.html")){
            result = page_path + "/" + lang + "/" + file;
        }

        if(piutils::chkfile(result)){
            logger::log(logger::LLOG::INFO, "WebW", std::string(__func__) + " Found. Page: " + result);
            return std::make_pair(std::string(http::web::WebSettings::mime_html), result);
        }

        logger::log(logger::LLOG::INFO, "WebW", std::string(__func__) + " Page not found: " + result);
        return std::make_pair(std::string(), std::string());
    }


    /**
     * @brief
     *
     * @param html
     * @return const std::string
     */
    virtual const std::string html_post_processing(const std::string& page_name, const std::string& html) override{
        logger::log(logger::LLOG::DEBUG, "WebW", std::string(__func__) + " Page:" + page_name);

        if(page_name.find("default") != std::string::npos || page_name.find("status") != std::string::npos ){
            auto ctxt = GET_ENV(weather::Context);

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
                char buff[64];
                Measurement data = ctxt->data;
                std::string lang = ctxt->get_language();

                sprintf(buff, "%.2f %c", data.temp(ctxt->show_temperature_in_celcius()), (ctxt->show_temperature_in_celcius() ? 'C' : 'F'));
                values["<!--{Temperature}-->"] = buff;

                sprintf(buff, "%.2f", data.si7021_humidity);
                values["<!--{Humidity}-->"] = buff;

                sprintf(buff, "%.2f", data.bmp280_pressure);
                values["<!--{Pressure}-->"] = buff;

                sprintf(buff, "%d", data.tsl2651_lux);
                values["<!--{Luximity}-->"] = buff;

                values["<!--{CO2}-->"] = std::to_string(data.spg30_co2);
                values["<!--{TVOC}-->"] = std::to_string(data.spg30_tvoc);

                int co2_level = ctxt->get_CO2_level();
                values["<!--{CO2_Level}-->"] = prepare_level_name(ctxt->get_level_label(co2_level, lang), co2_level);

                int tvoc_level = ctxt->get_TVOC_level();
                values["<!--{TVOC_Level}-->"] = prepare_level_name(ctxt->get_level_label(tvoc_level, lang), tvoc_level);
            }

            const std::string _new_page = piutils::webutils::WebUtils::replace_values(html, values);
            return _new_page;
        }

        return html;
    }

    /**
     * @brief Return the current weather status as a JSON document.
     *
     * Exposes the same runtime data used to render the HTML pages (network
     * info, uptime, latest sensor measurement, CO2/TVOC air-quality levels)
     * through the GET /api/status endpoint.
     *
     * @return const std::string JSON document
     */
    virtual const std::string get_status_json() override {
        logger::log(logger::LLOG::DEBUG, "WebW", std::string(__func__));

        auto ctxt = GET_ENV(weather::Context);
        Measurement data = ctxt->data;
        std::string lang = ctxt->get_language();
        const bool celcius = ctxt->show_temperature_in_celcius();

        const int co2_level = ctxt->get_CO2_level();
        const int tvoc_level = ctxt->get_TVOC_level();

        return prepare_output(
            "{"
                "\"version\":\"%s\","
                "\"language\":\"%s\","
                "\"uptime\":\"%s\","
                "\"net\":{\"ip4\":\"%s\",\"ip6\":\"%s\"},"
                "\"temperature_unit\":\"%c\","
                "\"measurement\":{"
                    "\"temperature\":%.2f,"
                    "\"humidity\":%.2f,"
                    "\"pressure\":%.2f,"
                    "\"luximity\":%u,"
                    "\"co2\":%u,"
                    "\"tvoc\":%u,"
                    "\"altitude\":%.2f"
                "},"
                "\"levels\":{"
                    "\"co2\":{\"value\":%d,\"label\":\"%s\"},"
                    "\"tvoc\":{\"value\":%d,\"label\":\"%s\"}"
                "}"
            "}",
            json_escape(ctxt->version).c_str(),
            json_escape(lang).c_str(),
            json_escape(ctxt->get_uptime()).c_str(),
            json_escape(ctxt->ip4_address).c_str(),
            json_escape(ctxt->ip6_address).c_str(),
            (celcius ? 'C' : 'F'),
            data.temp(celcius),
            data.si7021_humidity,
            data.bmp280_pressure,
            (unsigned)data.tsl2651_lux,
            (unsigned)data.spg30_co2,
            (unsigned)data.spg30_tvoc,
            data.bmp280_altitude,
            co2_level, json_escape(ctxt->get_level_label(co2_level, lang)).c_str(),
            tvoc_level, json_escape(ctxt->get_level_label(tvoc_level, lang)).c_str()
        );
    }

protected:
    /*
    * Refuse to delete the file FStorage currently has open for writing -
    * removing it from under an open fd would silently drop future writes
    * until the next daily rollover.
    */
    virtual bool is_file_delete_blocked(const std::string& full_path, const std::string& name) override {
        auto ctxt = GET_ENV(weather::Context);
        return ctxt->use_file_storage() && ctxt->_fstorage &&
               full_path == ctxt->_fstorage->current_file_path();
    }

    /*
    * Keep FStorage's data files list CSV in sync with files removed via the Web API.
    */
    virtual void on_file_deleted(const std::string& full_path, const std::string& name) override {
        auto ctxt = GET_ENV(weather::Context);
        if(ctxt->use_file_storage() && ctxt->_fstorage){
            ctxt->_fstorage->remove_data_files_list(full_path);
        }
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
