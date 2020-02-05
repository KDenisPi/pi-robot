/*
 * MosquittoClient.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: Denis Kudia
 */

#include <logger.h>
#include "MosquittoClient.h"

namespace mqtt {

const char TAG[] = "mosqt";

/*
* Constructor
*/
MosquittoClient::MosquittoClient(const std::string& client_id) : _client_id(client_id), m_connected(false) {
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Started ");

    // Initilize library
    int res = mosquitto_lib_init();
    MOSQ_CODE(res)

    //Get library version
    lib_version();

    //Initilize Mosquitto data
    init_data();
}

MosquittoClient::~MosquittoClient(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Started ");

    cl_disconnect();

    //release data
    clear_data();

    int res = mosquitto_lib_cleanup();
}

/*
* Get Mosquitto lib version
*/
void MosquittoClient::lib_version(){
    int res = mosquitto_lib_version(&_major, &_minor, &_revision);
    MOSQ_CODE(res)

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Version: [" + std::to_string(_major) + "." + std::to_string(_minor) + "-" + std::to_string(_revision));
}


/*
*
*/
const int MosquittoClient::cl_connect(const MqttServerInfo& conf){
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

    res = connect_async(conf.host(), conf.port(), conf.keepalive());

    //start network connection loop
    loop_start();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
    return res;
}

/*
*
*/
const int MosquittoClient::cl_disconnect(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));
    int res =  disconnect();
    if(res == MOSQ_ERR_SUCCESS){
        set_connected(false);
    }

    return res;
}

/*
*
*/
const std::string MosquittoClient::cl_get_version() const {
    return std::to_string(_major) + "." + std::to_string(_minor) + "." + std::to_string(_revision);
}

const int MosquittoClient::cl_publish(const std::string& topic, const std::string& payload){
    return publish(topic, payload, m_qos);
}

const int MosquittoClient::cl_subscribe(const std::string& topic){
    return subscribe(topic, m_qos);
}

const int MosquittoClient::cl_unsubscribe(const std::string& topic){
    return unsubscribe(topic);
}
const int MosquittoClient::cl_unsubscribe_all(){
    return 0;
}



/*
* Callback for on connect
*/
void MosquittoClient::on_connect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc));

    if(rc == MOSQ_ERR_SUCCESS){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc) + " Set connected: true");
        set_connected(true);
        cl_notify(MQTT_CONNECT, MQTT_ERROR_SUCCESS);
    }
    else{
        cl_notify(MQTT_CONNECT, (rc == MOSQ_ERR_INVAL ? MQTT_ERROR_INVAL : MQTT_ERROR_FAILED));
        set_connected(false);

        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc));

        err_conn_inc();
        if(rc != MOSQ_ERR_INVAL && !is_max_err_conn()){
           logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Trying to reconnect...");
           reconnect_async();
        }
    }

}

/*
* Callback for on connect flags
*/
void MosquittoClient::on_connect_flags(int rc, int flags){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect Code: " + cl_get_errname(rc) + " Flags:" + std::to_string(flags));

    if(rc == MOSQ_ERR_SUCCESS){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_connect_flags Code: " + cl_get_errname(rc) + " Set connected: true");
        set_connected(true);
        cl_notify(MQTT_CONNECT, MQTT_ERROR_SUCCESS);
    }
    else{
        cl_notify(MQTT_CONNECT, (rc == MOSQ_ERR_INVAL ? MQTT_ERROR_INVAL : MQTT_ERROR_FAILED));
        set_connected(false);

        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " on_connect_flags Code: " + cl_get_errname(rc) + " Flags:" + std::to_string(flags));

        err_conn_inc();
        if(rc != MOSQ_ERR_INVAL && !is_max_err_conn()){
           logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Trying to reconnect...");
           reconnect_async();
        }
    }

}


/*
* Callback for on disconnect
*/
void MosquittoClient::on_disconnect(int rc){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_disconnect Code: " + cl_get_errname(rc));

    set_connected(false);
    cl_notify(MQTT_DISCONNECT, (rc == MOSQ_ERR_SUCCESS ? MQTT_ERROR_SUCCESS : MQTT_ERROR_FAILED));

}

/*
* Callback for on publish
*/
void MosquittoClient::on_publish(int mid){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " on_publish MID: " + std::to_string(mid));

}

/*
* Callback for on subscribe
*/
void MosquittoClient::on_subscribe(int mid, int qos_count, const int * granted_qos){
    //possible race condition here
    std::string topic = subscription_by_mid(mid, false);
    if(topic.length() > 0){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " MID: " + std::to_string(mid) +
            " Topic: " + topic + " QOS Requested: " + std::to_string(qos_count) + " Granted: " + std::to_string(granted_qos[0]));
    }
}

/*
* Callback for on unsubscribe
*/
void MosquittoClient::on_unsubscribe(int mid){
    std::string topic = subscription_by_mid(mid, true);
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " MID: " + std::to_string(mid) + " Topic: " + topic);
}

/*
*
*/
void MosquittoClient::on_message(const struct mosquitto_message * message) {
    const std::string topic = message->topic;
    std::string msg = (char*)message->payload;
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Topic: " + topic + " MID: " + std::to_string(message->mid) + " Msg [" + msg + "]");
}


/*
* Callback for on logging from mosquitto library
*/
void MosquittoClient::on_log(int level, const char * str){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Level: " + std::to_string(level) + " Msg: " + str);
}

} // end namespace mqtt

