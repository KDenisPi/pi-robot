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
        const std::string url = "http://0.0.0.0:" + sport;
        mg_http_listen(&mgr, url.c_str(), WebSettings::html_pages, NULL);
    }

    /*
    *
    */
   virtual bool initialize() override {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));
        mg_mgr_init(&mgr);  // Initialise event manager
        mgr.userdata = (void*)this;
        return true;
   }

    /*
    *
    */
    virtual ~WebSettings() {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));
        mg_mgr_free(&mgr);
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

/**
 * @brief
 *
 * @param c
 * @param ev
 * @param ev_data
 */
    static void html_pages(struct mg_connection *c, int ev, void *ev_data) {

        if (ev == MG_EV_HTTP_MSG) {
            struct mg_http_message *hm = (struct mg_http_message *) ev_data;
            const auto srv = static_cast<WebSettings*>(c->mgr->userdata);

            if(mg_match(hm->uri, mg_str("/data"), NULL) ||
                mg_match(hm->uri, mg_str(".json"), NULL) ||
                mg_match(hm->uri, mg_str(".csv"),NULL)
                ){
                return srv->data_files(c, hm);
            }

            auto page_info = srv->get_page(hm);
            if(page_info.second.size()==0){
                return srv->file_not_found(c, hm);
            }

        }
        else {
            mg_http_reply(c, 503, NULL, "\n");
        }
/*

        mg_send_header(conn, "Content-Type", page_info.first.c_str());
        //mg_send_header(conn, "Content-Length", std::to_string(page_info.second.length()).c_str());
        mg_send_data(conn, page_info.second.c_str(), page_info.second.size());

    */
    }

    /*
    *
    */
    static void worker(WebSettings* p){
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " started");

        while(!p->is_stop_signal()){
            mg_mgr_poll(&p->mgr, 1000);
        }

        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " finished");
    }

   /**
    * @brief Get the page object
    *   Should be overloaded
    *
    * @param conn
    * @return const std::pair<std::string, std::string>
    */
    virtual const std::pair<std::string, std::string> get_page(const struct mg_http_message *hm) {
        return std::make_pair("", "");
    }

    /**
     * @brief
     *
     * @param conn
     */
    virtual void data_files(struct mg_connection *conn, const struct mg_http_message *hm){
       mg_http_reply(conn, 200, "Content-Type: text/plain\r\n", "No such files\n");
    }

    static void file_not_found(struct mg_connection *conn, const struct mg_http_message *hm){
       mg_http_reply(conn, 404, "Content-Type: text/html\r\n",
        "Page not found!<br> Requested URI is [%s], query string is [%s]\n", hm->uri.buf, (hm->query.len == 0 ? "None" : hm->query.buf));
    }

    /**
     * @brief Prepare list of IP addresses
     *
     * @return const std::string
     */
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

 protected:
    struct mg_mgr mgr;  // Mongoose event manager. Holds all connections

    //data interface
    std::shared_ptr<smachine::StateMachineItf> _itf;
};

}//namespace web
}//namespace weather

#endif
