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
* Callback for on connect
*/
void MosquittoClient::on_connect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client connected. Code: " + std::to_string(rc));
    return;
}

/*
* Callback for on disconnect
*/
void MosquittoClient::on_disconnect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client disconnected. Code: " + std::to_string(rc));
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
    return;
}    


}

