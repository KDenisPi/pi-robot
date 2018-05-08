/*
* WebSettings.h
*
* Web interface for Weather settings and status
*
*/
#ifndef WEATHER_WEB_SETTINGS_H
#define WEATHER_WEB_SETTINGS_H

#include "mongoose.h"
#include "Threaded.h"
#include "logger.h"
#include "web_utils.h"
#include "networkinfo.h"

namespace weather {

using PageContent = std::string;

class WebSettings : public piutils::Threaded {

public:
    WebSettings(const uint16_t port) {
        std::string sport = (port == 0 ? "8080" : std::to_string(port));
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Port: " + sport);

        _server = mg_create_server(this);
        mg_set_option(_server, "listening_port", sport.c_str());
        mg_add_uri_handler(_server, "/", WebSettings::index_html);
    }

    /*
    *
    */
    virtual ~WebSettings() {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));
    }

    /*
    *
    */
    void start(){
        piutils::Threaded::start<weather::WebSettings>(this);
    }

    /*
    *
    */
    static int index_html(struct mg_connection *conn) {

        mg_send_header(conn, "Content-Type:", "text/html; charset=utf-8");
        //try to load page content
        PageContent page = piutils::webutils::WebUtils::load_page("./web/status.html");
        //check loaded size
        if(page.size()==0){
            mg_send_status(conn, 404);
            mg_printf_data(conn, "Page not found!<br> Requested URI is [%s], query string is [%s]<br>Local path: ./web/status.html",
                 conn->uri,
                 conn->query_string == NULL ? "(none)" : conn->query_string);

            return 0;        
        }

        const std::string ip_info =   static_cast<WebSettings*>(conn->server_param)->prepare_ip_list();
        const std::string new_page = piutils::webutils::WebUtils::replace_value(page, "{NetInfo}", ip_info);
        mg_send_data(conn, new_page.c_str(), new_page.size());
        /*
        mg_printf_data(conn, "Hello! Requested URI is [%s], query string is [%s]",
                 conn->uri,
                 conn->query_string == NULL ? "(none)" : conn->query_string);
        */

        return 0;
    }


    /*
    *
    */
    static void worker(WebSettings* p){
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " started");

        while(!p->is_stop_signal()){
            mg_poll_server(p->_server, 1000);
        }

        mg_destroy_server(&p->_server);

        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " finished");
    }

private:

    //Prepare list of IP addresses
    const std::string prepare_ip_list(){  
        piutils::netinfo::NetInfo netInfo;
        std::string result;

        netInfo.load_ip_list();

        std::list<piutils::netinfo::ItfInfo> itf_ip = netInfo.get_default_ip(piutils::netinfo::IpType::IP_V4, "enp");
        std::list<piutils::netinfo::ItfInfo> itf_ip_wlan = netInfo.get_default_ip(piutils::netinfo::IpType::IP_V4, "wl");
        //std::list<piutils::netinfo::ItfInfo> itf_ip_v6 = netInfo.get_default_ip(piutils::netinfo::IpType::IP_V6);
        itf_ip.splice(itf_ip.end(), itf_ip_wlan);

        if(itf_ip.size() == 0){
            return "Not detected";
        }

        for(auto itfip : itf_ip){
            result += "Interface: " + itfip.first + " IP: " + itfip.second + "<br>";
        }

        return result;
    }

    struct mg_server* _server;
};

}//namespace weather

#endif
