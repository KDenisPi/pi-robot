/*
* WebSettings.h
*
* Web interface for settings and status
*
*/
#ifndef HTTP_WEB_SETTINGS_H
#define HTTP_WEB_SETTINGS_H

#include <utility>
#include <arpa/inet.h>

#include "mongoose.h"

#include "Threaded.h"
#include "logger.h"
#include "web_utils.h"
#include "networkinfo.h"

namespace http {
namespace web {

class WebSettingsItf {
public:
    WebSettingsItf() {}
    virtual ~WebSettingsItf() {}

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    virtual bool initialize() {return true;}

    /**
     * @brief
     *
     */
    virtual void start() {}

    /**
     * @brief
     *
     */
    virtual void stop() {}

    /**
     * @brief Get the page by URI object
     *
     * @param uri
     * @return const std::pair<std::string, std::string>
     */
    virtual const std::pair<std::string, std::string> get_page_by_URI(const std::string& uri) {return std::make_pair("", "");}
};


const char* mime_html = "Content-Type: text/html; charset=utf-8\r\n";
const char* mime_js = "Content-Type: application/javascript; charset=utf-8\r\n";
const char* mime_plain = "Content-Type: text/plain; charset=utf-8\r\n";
const char* mime_css = "Content-Type: text/css; charset=utf-8\r\n";
const char* mime_json = "Content-Type: application/json; charset=utf-8\r\n";
const char* mime_csv = "Content-Type: text/csv; charset=utf-8\r\n";

class WebSettings : public piutils::Threaded, public WebSettingsItf {

public:
    WebSettings(const uint16_t port){
        std::string sport = (port == 0 ? "8080" : std::to_string(port));
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Port: " + sport);

        init();

        initialize();

        const std::string url = "http://127.0.0.1:" + sport;
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Listener: " + url);
        mg_http_listen(&mgr, url.c_str(), WebSettings::html_pages, NULL);
    }

    /**
     * @brief Internal initialization function
     *
     * @return true
     * @return false
     */
    bool init() {
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__));
        mg_mgr_init(&mgr);  // Initialise event manager
        mgr.userdata = (void*)this;
        return true;
    }

    /**
     * @brief Destroy the Web Settings object
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

    static const std::string mg_addr2str(const struct mg_addr& addr){
        return piutils::netinfo::NetUtils::ip2str(addr.ip, addr.is_ip6) + ":" + std::to_string(ntohs(addr.port));
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

            logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Request: " +  mg_addr2str(c->loc));

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
            //logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " EvType: " +  std::to_string(ev));
            //mg_http_reply(c, 503, NULL, "");
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
     *
     * @param hm
     * @return const std::pair<std::string, std::string>
     */
    const std::pair<std::string, std::string> get_page(const struct mg_http_message *hm) {
        const std::string uri = std::string(hm->uri.buf, hm->uri.len);
        return get_page_by_URI(uri);
    }

    /**
     * @brief Get the page by URI object
     *
     * @param uri
     * @return const std::pair<std::string, std::string>
     */
    virtual const std::pair<std::string, std::string> get_page_by_URI(const std::string& uri) override {
        return std::make_pair("", "");
    }


    /**
     * @brief
     *
     * @param conn
     */
    virtual void data_files(struct mg_connection *conn, const struct mg_http_message *hm){
       mg_http_reply(conn, 200, mime_plain, "No such files\n");
    }

    /**
     * @brief
     *
     * @param conn
     * @param hm
     */
    static void file_not_found(struct mg_connection *conn, const struct mg_http_message *hm){
       mg_http_reply(conn, 404, mime_html,
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
};

}//namespace web
}//namespace weather

#endif
