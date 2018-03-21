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

namespace weather {

class WebSettings : public piutils::Threaded {

public:
    WebSettings(const uint16_t port) {

        std::string sport = (port == 0 ? "8080" : std::to_string(port));

        _server = mg_create_server(this);
        mg_set_option(_server, "listening_port", sport.c_str());
        mg_add_uri_handler(_server, "/", WebSettings::index_html);
    }

    /*
    *
    */
    virtual ~WebSettings() {}

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

        mg_printf_data(conn, "Hello! Requested URI is [%s], query string is [%s]",
                 conn->uri,
                 conn->query_string == NULL ? "(none)" : conn->query_string);

        return 0;
    }


    /*
    *
    */
    static void worker(WebSettings* p){

        while(p->is_stop_signal()){
            mg_poll_server(p->_server, 1000);
        }

        mg_destroy_server(&p->_server);
    }

    struct mg_server* _server;
};

}//namespace weather

#endif