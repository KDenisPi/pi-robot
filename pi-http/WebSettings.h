/*
* WebSettings.h
*
* Web interface for settings and status
*
*/
#ifndef HTTP_WEB_SETTINGS_H
#define HTTP_WEB_SETTINGS_H

#include <utility>

#include "mongoose.h"
#include "Threaded.h"
#include "logger.h"
#include "web_utils.h"
#include "networkinfo.h"
#include "StateMachineItf.h"

namespace http {
namespace web {

class WebSettingsItf {
public:
    WebSettingsItf() {}
    virtual ~WebSettingsItf() {}

    virtual bool initialize() {return true;}
    virtual void start() {}
    virtual void stop() {}
};


const char* mime_html = "text/html; charset=utf-8";
const char* mime_js = "application/javascript; charset=utf-8";
const char* mime_plain = "text/plain; charset=utf-8";
const char* mime_css = "text/css; charset=utf-8";
const char* mime_json = "application/json; charset=utf-8";
const char* mime_csv = "text/csv; charset=utf-8";

class WebSettings : public piutils::Threaded, public WebSettingsItf {

public:
    WebSettings(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) : _itf(itf) {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));

        initialize();

        std::string sport = (port == 0 ? "8080" : std::to_string(port));
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Port: " + sport);
        mg_set_option(_server, "listening_port", sport.c_str());

        mg_add_uri_handler(_server, "/", WebSettings::html_pages);
    }

    /*
    *
    */
   virtual bool initialize() override {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));
        _server = mg_create_server(this);
        return true;
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
    virtual void start() override{
        piutils::Threaded::start<http::web::WebSettings>(this);
    }

    virtual void stop() override {
        piutils::Threaded::stop();
    }

    /*
    *
    */
    static int html_pages(struct mg_connection *conn) {

        if(std::strstr(conn->uri, "/data") != NULL || std::strstr(conn->uri, ".json") != NULL || std::strstr(conn->uri, ".csv") != NULL){
            return static_cast<WebSettings*>(conn->server_param)->data_files(conn);
        }

        auto page_info = static_cast<WebSettings*>(conn->server_param)->get_page(conn);

        //check loaded size
        if(page_info.second.size()==0){
            mg_send_header(conn, "Content-Type", "text/html; charset=utf-8");
            mg_send_status(conn, 404);
            mg_printf_data(conn, "Page not found!<br> Requested URI is [%s], query string is [%s]",
                 conn->uri,
                 conn->query_string == NULL ? "(none)" : conn->query_string);

            return 0;
        }

        mg_send_header(conn, "Content-Type", page_info.first.c_str());
        //mg_send_header(conn, "Content-Length", std::to_string(page_info.second.length()).c_str());
        mg_send_data(conn, page_info.second.c_str(), page_info.second.size());
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

    /*
    *   Should be overloaded
    */
    virtual const std::pair<std::string, std::string> get_page(const struct mg_connection *conn) {
        return std::make_pair("", "");
    }

    virtual int data_files(struct mg_connection *conn) { return 0;}

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

	template<class T> std::shared_ptr<T> get_context(){
		return std::static_pointer_cast<T>(_itf->get_env());
	}

 protected:
    struct mg_server* _server;

    //data interface
    std::shared_ptr<smachine::StateMachineItf> _itf;
};

}//namespace web
}//namespace weather

#endif
