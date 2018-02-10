/*
 * MosquittoClient.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: Denis Kudia
 */

#include <logger.h>
#include "MosquittoClient.h"

namespace mqqt {

const char TAG[] = "mosqt";

/*
* Constructor
*/
MosquittoClient::MosquittoClient(const char* clientID) :
    mosqpp::mosquittopp(clientID, true), m_connected(false) {
  logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Started ");

}

MosquittoClient::~MosquittoClient(){
  logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Started ");
  cl_disconnect();
  loop_stop(true);
}


/*
*
*/
const int MosquittoClient::cl_connect(const MqqtServerInfo& conf){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Host: " + conf.host() + " Port: " + std::to_string(conf.port()) + " TLS: " + std::to_string(conf.is_tls()));

    m_qos = conf.qos();
    reconnect_delay_set(reconnect_delay, reconnect_delay_max, false);

    int res;
    //TLS sapport
    if(conf.is_tls()){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " TLS: " + std::to_string(conf.is_tls()) + 
            " CA file: " + conf.get_cafile() + " Insecure: " + std::to_string(conf.is_tls_insecure()) + 
            " Version: " + conf.get_tls_version());
        
        res = tls_set(conf.get_cafile().c_str());
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " TLS set: " + cl_get_errname(res) + " CA file: " + conf.get_cafile());

        res = tls_insecure_set((conf.is_tls_insecure() ? true : false));
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " TLS Insecure set: " + cl_get_errname(res) + " TLS insecure: " + std::to_string(conf.is_tls_insecure()));
        
        res = tls_opts_set(0, conf.get_tls_version().c_str());
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " TLS option set: " + cl_get_errname(res) + " TLS version: " + conf.get_tls_version());
    }

    res =  connect_async(conf.host(), conf.port(), conf.keepalive());
    loop_start();

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Finished: " + cl_get_errname(res));
    return res;
}

/*
*
*/
const int MosquittoClient::cl_disconnect(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));
    int res =  disconnect();
    if(res == MOSQ_ERR_SUCCESS)
        set_connected(false);

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Result: " + cl_get_errname(res));
    return res;
}

/*
*
*/
const std::string MosquittoClient::cl_get_version() const {
    int mjr=0, mnr=0, rev=0;
    mosqpp::lib_version(&mjr, &mnr, &rev);
    return std::to_string(mjr) + "." + std::to_string(mnr) + "." + std::to_string(rev);
}


const int MosquittoClient::cl_publish(int* mid, const std::string& topic, const std::string& payload){
    return publish(mid, topic.c_str(), payload.length(), payload.c_str(), m_qos);
}
const int MosquittoClient::cl_publish(int* mid, const std::string& topic, const int payloadsize, const void* payload){
    return publish(mid, topic.c_str(), payloadsize, payload, m_qos);
}

/*
* Callback for on connect
*/
void MosquittoClient::on_connect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc));

    if(rc == MOSQ_ERR_SUCCESS){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc) + " Set connected: true");        
        set_connected(true);
        cl_notify(MQQT_CONNECT, MQQT_ERROR_SUCCESS);
    }
    else{
        cl_notify(MQQT_CONNECT, (rc == MOSQ_ERR_INVAL ? MQQT_ERROR_INVAL : MQQT_ERROR_FAILED));
        set_connected(false);

        err_conn_inc();
        if(rc != MOSQ_ERR_INVAL && !is_max_err_conn()){
           logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Trying to reconnect...");
           reconnect_async();
        }
    }
    return;
}

/*
* Callback for on disconnect
*/
void MosquittoClient::on_disconnect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_disconnect Code: " + cl_get_errname(rc));

    set_connected(false);
    cl_notify(MQQT_DISCONNECT, (rc == MOSQ_ERR_SUCCESS ? MQQT_ERROR_SUCCESS : MQQT_ERROR_FAILED));
    return;
}

/*
* Callback for on publish
*/
void MosquittoClient::on_publish(int mid){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_publish MID: " + std::to_string(mid));
    return;
}

/*
* Callback for on subscribe
*/
void MosquittoClient::on_subscribe(int mid, int qos_count, const int * granted_qos){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " on_subscribe MID: " + std::to_string(mid) +
        " QOS Requested: " + std::to_string(qos_count) + " Granted: " + std::to_string(granted_qos[0]));
    return;
}

/*
* Callback for on unsubscribe
*/
void MosquittoClient::on_unsubscribe(int /*mid*/){
    return;
}

/*
* Callback for on logging from mosquitto library
*/
void MosquittoClient::on_log(int level, const char * str){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Level: " + std::to_string(level) + " Msg: " + str);
    return;
}

/*
* Callback for on error
*/
void MosquittoClient::on_error(){
    //TBD: Add error processing there
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    return;
}    

} // end namespace mqqt

