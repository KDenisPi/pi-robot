/*
 * MosquittoClient.h
 *
 *  Created on: Apr 14, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_MOSQUITTO_CL_H_
#define PI_MQTT_MOSQUITTO_CL_H_

#include <functional>
#include <memory>
#include <map>
#include <mosquitto.h>

#include "MqttDefines.h"
#include "MqttClientItf.h"

namespace mqtt {

using sub_info = std::pair<std::string, int>;

#define MOSQ_CODE(code){\
    if(code == MOSQ_ERR_SUCCESS){\
        logger::log(logger::LLOG::DEBUG, "mosqt", std::string(__func__) + " Success");\
    }\
    else{\
        if(code == MOSQ_ERR_ERRNO){\
            logger::log(logger::LLOG::ERROR, "mosqt", std::string(__func__) + " ERRNO " + std::to_string(errno));\
        }\
        else\
            logger::log(logger::LLOG::ERROR, "mosqt", std::string(__func__) + " Error " + cl_get_errname(code));\
    }\
}


class MosquittoClient : public MqttClientItf
{
public:
    MosquittoClient(const std::string& client_id);
    virtual ~MosquittoClient();

    virtual const int cl_connect(const MqttServerInfo& conf);
    virtual const int cl_disconnect();
    virtual const std::string cl_get_version() const;
    virtual const int cl_publish(const std::string& topic, const std::string& payload) override;
    virtual const int cl_subscribe(const std::string& topic) override;
    virtual const int cl_unsubscribe(const std::string& topic) override;
    virtual const int cl_unsubscribe_all() override;

    virtual void on_connect(int rc);
    /*
    * on_connect_flags
    *
    * Set the connect callback. This is called when the broker sends a CONNACK message in response to a connection.
    *
    *  flags - the connect flags.
    *
    * * 0 - success
    * * 1 - connection refused (unacceptable protocol version)
    * * 2 - connection refused (identifier rejected)
    * * 3 - connection refused (broker unavailable)
    * * 4-255 - reserved for future use
    */
    virtual void on_connect_flags(int rc, int flags);
    virtual void on_disconnect(int rc);
    virtual void on_publish(int mid);
    virtual void on_message(const struct mosquitto_message * message);
    virtual void on_subscribe(int mid, int qos_count, const int * granted_qos);
    virtual void on_unsubscribe(int mid);
    virtual void on_log(int level, const char * str);

    /*
    * Callback functions
    */
    static void callback_connect(struct mosquitto *mosq, void* data, int rc){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_connect(rc);
    }

    static void callback_connect_flags(struct mosquitto *mosq, void* data, int rc, int flags){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_connect_flags(rc, flags);
    }

    static void callback_disconnect(struct mosquitto *mosq, void* data, int rc){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_disconnect(rc);
    }

    static void callback_publish(struct mosquitto *mosq, void* data, int rc){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_publish(rc);
    }

    static void callback_message(struct mosquitto *mosq, void* data, const struct mosquitto_message * msg){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_message(msg);
    }

    static void callback_subscribe(struct mosquitto *mosq, void* data, int mid, int qos_count, const int * granted_qos){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_subscribe(mid, qos_count, granted_qos);
    }

    static void callback_unsubscribe(struct mosquitto *mosq, void* data, int rc){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_unsubscribe(rc);
    }

    static void callback_log(struct mosquitto *mosq, void* data, int level, const char * str){
        MosquittoClient* clnt = static_cast<MosquittoClient*>(data);
        clnt->on_log(level, str);
    }

    unsigned int reconnect_delay = 2;
    unsigned int reconnect_delay_max = 6;

    const std::string cl_get_errname(const int code) const{
        switch(code){
            case MOSQ_ERR_CONN_PENDING: return "Connection pending (-1)";
            case MOSQ_ERR_SUCCESS: return "Success (0)";
            case MOSQ_ERR_NOMEM: return "No memory (1)";
            case MOSQ_ERR_PROTOCOL: return "Protocol (2)";
            case MOSQ_ERR_INVAL: return "Invalid value (3)";
            case MOSQ_ERR_NO_CONN: return "No Connection (4)";
            case MOSQ_ERR_CONN_REFUSED: return "Connection refused (5)";
            case MOSQ_ERR_NOT_FOUND: return "Not found (6)";
            case MOSQ_ERR_CONN_LOST: return "Connection lost (7)";
            case MOSQ_ERR_TLS: return "TLS (8)";
            case MOSQ_ERR_PAYLOAD_SIZE: return "Payload size (9)";
            case MOSQ_ERR_NOT_SUPPORTED: return "Not supported (10)";
            case MOSQ_ERR_AUTH: return "Authentication (11)";
            case MOSQ_ERR_ACL_DENIED: return "ACL denied (12)";
            case MOSQ_ERR_UNKNOWN: return "Unknown (13)";
            case MOSQ_ERR_ERRNO: return "Err NO (14)";
            case MOSQ_ERR_EAI: return "EAI (15)";
            case MOSQ_ERR_PROXY: return "Proxy (16)";
            default: return std::to_string(code);
        }
    }

virtual bool const is_connected() const override { return m_connected; }

private:
    void lib_version();

    void set_connected(const bool connected){
        m_connected = connected;
    }

    void reconnect_delay_set(unsigned int reconnect_delay, unsigned int reconnect_delay_max, bool reconnect_exponential_backoff)
    {
	    int res = mosquitto_reconnect_delay_set(_mosq, reconnect_delay, reconnect_delay_max, reconnect_exponential_backoff);
        MOSQ_CODE(res)
    }

    int tls_set(const char *cafile, const char *capath = NULL, const char *certfile = NULL, const char *keyfile = NULL, int (*pw_callback)(char *buf, int size, int rwflag, void *userdata) = NULL)
    {
        int res = mosquitto_tls_set(_mosq, cafile, capath, certfile, keyfile, pw_callback);
        MOSQ_CODE(res)
        return res;
    }

    int tls_opts_set(int cert_reqs, const char *tls_version, const char *ciphers = NULL)
    {
        int res = mosquitto_tls_opts_set(_mosq, cert_reqs, tls_version, ciphers);
        MOSQ_CODE(res)
        return res;
    }

    int tls_insecure_set(bool value)
    {
        int res = mosquitto_tls_insecure_set(_mosq, value);
        MOSQ_CODE(res)
        return res;
    }

    int tls_psk_set(const char *psk, const char *identity, const char *ciphers)
    {
        int res = mosquitto_tls_psk_set(_mosq, psk, identity, ciphers);
        MOSQ_CODE(res)
        return res;
    }

    int connect_async(const char *host, int port, int keepalive)
    {
        int res = mosquitto_connect_async(_mosq, host, port, keepalive);
        MOSQ_CODE(res)
        return res;
    }

    int reconnect_async() {
        int res = mosquitto_reconnect_async(_mosq);
        MOSQ_CODE(res)
        return res;
    }

    int disconnect() {
        int res = mosquitto_disconnect(_mosq);
        MOSQ_CODE(res)
        return res;
    }

    int loop_start(){
        int res = mosquitto_loop_start(_mosq);
        MOSQ_CODE(res)
        return res;
    }

    int publish(const std::string& topic, const std::string& payload, int qos = 0, bool retain = false){
        int mid = 0;
        int res = mosquitto_publish(_mosq, &mid, topic.c_str(), payload.length(), payload.c_str(), qos, retain);
        MOSQ_CODE(res)

        //save information about published message (base on mid)
        if(res == mqtt::MQTT_ERROR_SUCCESS){

        }

        return res;
    }

    int subscribe(const std::string& sub, int qos = 0){
        int mid = 0;
        int res = mosquitto_subscribe(_mosq, &mid, sub.c_str(), qos);
        MOSQ_CODE(res)

        //save information about subscription (base on mid)
        if(res == mqtt::MQTT_ERROR_SUCCESS){

            auto subi = this->_subscriptions.find(mid);
            if(subi != _subscriptions.end()){
                //TODO: we have such subscription already
            }
            else{
                _subscriptions.insert({mid, std::make_shared<std::pair<std::string, int>>(sub, mid)});
            }

        }

        return res;
    }

    int unsubscribe(const std::string& sub){
        int mid = 0;
        int res = mosquitto_unsubscribe(_mosq, &mid, sub.c_str());
        MOSQ_CODE(res)

        return res;
    }

    /*
    *
    */
    int init_data(){
        clear_data();

        // Initilize Mosquitto internal structure
        _mosq = mosquitto_new(_client_id.c_str(), false, (void*)this);
        if(_mosq == NULL){
            // Process error
            return errno;
        }

        mosquitto_connect_callback_set(_mosq, MosquittoClient::callback_connect);
        mosquitto_connect_with_flags_callback_set(_mosq, MosquittoClient::callback_connect_flags);
        mosquitto_disconnect_callback_set(_mosq, MosquittoClient::callback_disconnect);
        mosquitto_publish_callback_set(_mosq, MosquittoClient::callback_publish);
        mosquitto_message_callback_set(_mosq, MosquittoClient::callback_message);
        mosquitto_subscribe_callback_set(_mosq, MosquittoClient::callback_subscribe);
        mosquitto_unsubscribe_callback_set(_mosq, MosquittoClient::callback_unsubscribe);
        mosquitto_log_callback_set(_mosq, MosquittoClient::callback_log);

        return 0;
    }

    void clear_data(){
        if(_mosq == nullptr)
            return;

        mosquitto_destroy(_mosq);
        _mosq = nullptr;
    }


private:
    int m_qos;
    bool m_connected;
    std::string _client_id;

    int _major, _minor, _revision;
    struct mosquitto* _mosq = nullptr;

    //Map of subscriptions
    std::map<int, const std::shared_ptr<sub_info>> _subscriptions;
};

} /*end namespace mqtt*/

#endif