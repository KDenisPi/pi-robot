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
  loop_stop(true);
}


/*
*
*/
const int MosquittoClient::cl_connect(const MqqtServerInfo& conf){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));

    m_qos = conf.qos();
    reconnect_delay_set(reconnect_delay, reconnect_delay_max, false);
    int res =  connect_async(conf.host(), conf.port(), conf.keepalive());
    loop_start();

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Finished: " + cl_get_errname(res));
    return res;
}

/*
*
*/
const int MosquittoClient::cl_disconnect(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));
    return disconnect();
}

/*
*
*/
const std::string MosquittoClient::cl_get_version() const {
    int mjr=0, mnr=0, rev=0;
    mosqpp::lib_version(&mjr, &mnr, &rev);
    return std::to_string(mjr) + "." + std::to_string(mnr) + "." + std::to_string(rev);
}


const int MosquittoClient::cl_publish(int mid, std::string& topic, std::string& payload){
    int mmid = mid;
    return publish(&mmid, topic.c_str(), payload.length(), payload.c_str(), m_qos);
}

/*
* Callback for on connect
*/
void MosquittoClient::on_connect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client connected. Code: " + cl_get_errname(rc));

    if(rc == MOSQ_ERR_SUCCESS){
        cl_notify(MQQT_CONNECT, MQQT_ERROR_SUCCESS);
        m_connected = true;
    }
    else{
        cl_notify(MQQT_CONNECT, (rc == MOSQ_ERR_INVAL ? MQQT_ERROR_INVAL : MQQT_ERROR_FAILED));
        m_connected = false;

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
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client disconnected. Code: " + cl_get_errname(rc));

    m_connected = false;
    cl_notify(MQQT_DISCONNECT, (rc == MOSQ_ERR_SUCCESS ? MQQT_ERROR_SUCCESS : MQQT_ERROR_FAILED));
    loop_stop(false);
    return;
}

/*
* Callback for on publish
*/
void MosquittoClient::on_publish(int mid){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " message was published. MID: " + std::to_string(mid));
    return;
}

/*
* Callback for on subscribe
*/
void MosquittoClient::on_subscribe(int mid, int qos_count, const int * granted_qos){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Subscribd. MID: " + std::to_string(mid) +
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
void MosquittoClient::on_log(int /*level*/, const char * str){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + str);
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

