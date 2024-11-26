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
#include "smallthings.h"

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

using dir_map = std::map<std::string, std::string>;

class WebSettings : public piutils::Threaded, public WebSettingsItf {

public:
    WebSettings(const uint16_t port){
        std::string sport = (port == 0 ? "8080" : std::to_string(port));
        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Port: " + sport);

        init();

        initialize();

        const std::string url = "http://0.0.0.0:" + sport;
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

    static const size_t MAX_BUFF_SIZE = 4096;

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
     * @brief Convert struct mg_addr to string
     *
     * @param addr
     * @return const std::string
     */
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

            //data files JSON or CSV, located in /data folder
            if(mg_match(hm->uri, mg_str("/data/*"), NULL)){
                const auto uri = mg_str2str(hm->uri);
                logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Data request: " + uri);

                if(uri.find(".csv")>0 && srv->is_dir_map("data"))
                    return srv->data_files(c, hm, "data");
                else if(uri.find(".json")>0 && srv->is_dir_map("json"))
                    return srv->data_files(c, hm, "json");

                return srv->file_not_found(c, hm);
            }
            else if(mg_match(hm->uri, mg_str("/static/*"), NULL)){ //STATIC files, located in /static
                const auto uri = mg_str2str(hm->uri);
                logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " Static request: " + uri);
                if(srv->is_dir_map("static")){
                    return srv->data_files(c, hm, "static");
                }

                return srv->file_not_found(c, hm);
            }

            //HTML pages
            const auto page_info = srv->get_page(hm);
            if(!page_info.second.empty()){
                const std::string page = piutils::webutils::WebUtils::load_page(page_info.second);
                if(!page.empty())
                    return send_string(c, 200, page_info.first.c_str(), page);
                else //404 or 503 or something else
                    return srv->file_not_found(c, hm);
            }
            else{
                return srv->file_not_found(c, hm);
            }

        }
        else {
        }
    }

    /**
     * @brief
     *
     * @param p
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
        const auto file = uri_file(uri);
        const auto full_path = web_root + "/" + file;

        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " URI: " + uri + " File: " + full_path);

        if(piutils::chkfile(full_path)){
            return std::make_pair(std::string(mime_html), full_path);
        }
        return std::make_pair("", "");

    }


    /**
     * @brief
     *
     * @param conn
     */
    virtual void data_files(struct mg_connection *conn, struct mg_http_message *hm, const std::string& dir){
        struct mg_http_serve_opts opts;
        memset(&opts, 0, sizeof(mg_http_serve_opts));
        opts.mime_types = "html=text/html,htm=text/html,css=text/css,csv=text/csv,json=application/json,jpg=image/jpeg,png=image/png";

        const std::string uri = std::string(hm->uri.buf, hm->uri.len);
        const auto file = uri_file(uri);
        const auto full_path = dmaps[dir] + "/" + file;

        logger::log(logger::LLOG::DEBUG, "WEB", std::string(__func__) + " URI: " + uri + " File: " + full_path);

        if(piutils::chkfile(full_path)){
            mg_http_serve_file(conn, hm, full_path.c_str(), &opts);
        }
        else{
            file_not_found(conn, hm);
        }

        //mg_http_reply(conn, 200, mime_plain, "No such files\n");
    }

    /**
     * @brief
     *
     * @param conn
     * @param hm
     */
    static void file_not_found(struct mg_connection *conn, const struct mg_http_message *hm){
        auto body = prepare_output("Page not found!<br> Requested URI is [%s], query string is [%s]\n", mg_str2str(hm->uri).c_str(), (hm->query.len == 0 ? "None" : mg_str2str(hm->query).c_str()));
        mg_http_reply(conn, 404, mime_html, "%s", body.c_str());
    }

    /**
     * @brief
     *
     * @param conn
     * @param code
     * @param mime_type
     * @param body
     */
    static void send_string(struct mg_connection *conn, const int code, const char* mime_type, const std::string& body){
        mg_printf(conn, "HTTP/1.1 %u %s\r\n", code, http_status_code_str(code));
        mg_printf(conn, mime_type);
        mg_printf(conn, "Content-Length: %u\r\n\r\n", body.length());
        mg_printf(conn, "%s", body.c_str());
        conn->is_resp = 0;
    }

    /**
     * @brief
     *
     * @param fmt
     * @param ...
     * @return const std::string
     */
    static const std::string prepare_output(const char *fmt, ...){
        char buffer[MAX_BUFF_SIZE];
        va_list ap;
        va_start(ap, fmt);
        size_t len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
        va_end(ap);
        return std::string(buffer, len);
    }

    /**
     * @brief
     *
     * @param mstr
     * @return const std::string
     */
    static const std::string mg_str2str(const struct mg_str& mstr){
        return std::string(mstr.buf, mstr.len);
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

    /**
     * @brief
     *
     * @param status_code
     * @return const char*
     */
    static const char *http_status_code_str(int status_code) {
        switch (status_code) {
            case 100: return "Continue";
            case 101: return "Switching Protocols";
            case 102: return "Processing";
            case 200: return "OK";
            case 201: return "Created";
            case 202: return "Accepted";
            case 203: return "Non-authoritative Information";
            case 204: return "No Content";
            case 205: return "Reset Content";
            case 206: return "Partial Content";
            case 207: return "Multi-Status";
            case 208: return "Already Reported";
            case 226: return "IM Used";
            case 300: return "Multiple Choices";
            case 301: return "Moved Permanently";
            case 302: return "Found";
            case 303: return "See Other";
            case 304: return "Not Modified";
            case 305: return "Use Proxy";
            case 307: return "Temporary Redirect";
            case 308: return "Permanent Redirect";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 402: return "Payment Required";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 406: return "Not Acceptable";
            case 407: return "Proxy Authentication Required";
            case 408: return "Request Timeout";
            case 409: return "Conflict";
            case 410: return "Gone";
            case 411: return "Length Required";
            case 412: return "Precondition Failed";
            case 413: return "Payload Too Large";
            case 414: return "Request-URI Too Long";
            case 415: return "Unsupported Media Type";
            case 416: return "Requested Range Not Satisfiable";
            case 417: return "Expectation Failed";
            case 418: return "I'm a teapot";
            case 421: return "Misdirected Request";
            case 422: return "Unprocessable Entity";
            case 423: return "Locked";
            case 424: return "Failed Dependency";
            case 426: return "Upgrade Required";
            case 428: return "Precondition Required";
            case 429: return "Too Many Requests";
            case 431: return "Request Header Fields Too Large";
            case 444: return "Connection Closed Without Response";
            case 451: return "Unavailable For Legal Reasons";
            case 499: return "Client Closed Request";
            case 500: return "Internal Server Error";
            case 501: return "Not Implemented";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            case 504: return "Gateway Timeout";
            case 505: return "HTTP Version Not Supported";
            case 506: return "Variant Also Negotiates";
            case 507: return "Insufficient Storage";
            case 508: return "Loop Detected";
            case 510: return "Not Extended";
            case 511: return "Network Authentication Required";
            case 599: return "Network Connect Timeout Error";
            default: return "";
        }
    }

    /**
     * @brief
     *
     * @param dir
     * @param folder
     */
    void add_dir_map(const std::string& dir, const std::string& folder){
        dmaps[dir] = folder;
    }

    const bool is_dir_map(const std::string& dir) const{
        const auto dir_key = dmaps.find(dir);
        return (dir_key != dmaps.end());
    }

    /**
     * @brief Set the web root object
     *
     * @param root
     */
    void set_web_root(const std::string& root){
        if(!root.empty()){
            web_root = root;
        }
    }

    /**
     * @brief
     *
     * @param uri
     * @return const std::string
     */
    const std::string uri_file(const std::string& uri){
        auto pos = uri.find_last_of("/");
        return uri.substr(pos+1);
    }

 protected:
    struct mg_mgr mgr;  // Mongoose event manager. Holds all connections

    /*
    Map between internal folder name and filesystem folders
    Example:
        "data" =  "/opt/data"
    */
    dir_map dmaps;

    std::string web_root = "./";

};

}//namespace web
}//namespace weather

#endif
