/*
 * MqttClient.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_CLIENT_H_
#define PI_MQTT_CLIENT_H_

#include <memory>
#include <functional>
#include <queue>

#include "logger.h"

#include "MqttDefines.h"
#include "MqttClientItf.h"
#include "mqtt_object.h"

#include "Threaded.h"
#include "CircularBuffer.h"

namespace mqtt {


const char TAG_CL[] = "mqttcl";

using pub_info = std::pair<std::string,std::pair<int, std::string>>;

class MqttItf {
public:
    MqttItf() {}
    virtual ~MqttItf() {}

    virtual bool start() = 0;
    virtual void stop() = 0;

    virtual const MQTT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload) = 0;
    virtual const MQTT_CLIENT_ERROR subscribe(const std::string& topic, int qos = 0) = 0;
    virtual const MQTT_CLIENT_ERROR unsubscribe(const std::string& topic) = 0;
    virtual const MQTT_CLIENT_ERROR unsubscribe_all() = 0;
    virtual const bool is_connected() = 0;
};

/*
* MQTT client implementation
*/
template <class T>
class MqttClient : public piutils::Threaded, public MqttItf {
public:
    /*
    *
    */
    MqttClient<T>(const MqttServerInfo& conf) :
        m_conf(conf), m_max_size(100)
    {
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + m_conf.to_string() );

        m_mqttCl = std::make_shared<T>(m_conf.clientid());
        m_mqttCl->owner_notification = std::bind(&MqttClient::on_client, this, std::placeholders::_1, std::placeholders::_2);
        m_messages = std::make_shared<piutils::circbuff::CircularBuffer<std::shared_ptr<MqttObject>>>(m_max_size);

        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Version: " + get_version());
    }

    /*
    *
    */
    virtual ~MqttClient(){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started");

        disconnect();
        m_mqttCl.reset();
    }

    /*
    *
    */
    const int connect(){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Started");
        return m_mqttCl->cl_connect(m_conf);
    }

    virtual const bool is_connected() override {
        return m_mqttCl->is_connected();
    }

    /*
    * Temporal: Wait for processing.
    */
    void wait(){
        piutils::Threaded::stop(false);
    }

    /*
    *
    */
    virtual const MQTT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload) override {
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Topic: [" + topic + "]");

        //do nothing if client stopped already
        if(is_stop_signal()){
            logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Ignored");
            return MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS;
        }

        std::shared_ptr<MqttObject> item = std::make_shared<MqttObject>(topic, payload);
        m_messages->put(std::move(item));
        cv.notify_one();

        return MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS;
    }

    virtual const MQTT_CLIENT_ERROR subscribe(const std::string& topic, int qos = 0) override {
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Topic: [" + topic + "]");

        //do nothing if client stopped already
        if(is_stop_signal()){
            logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Ignored");
            return MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS;
        }

        int res = m_mqttCl->cl_subscribe(topic);
        return (res == 0 ? MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS : MQTT_CLIENT_ERROR::MQTT_ERROR_FAILED);
    }

    virtual const MQTT_CLIENT_ERROR unsubscribe(const std::string& topic){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Topic: [" + topic + "]");
        int res = m_mqttCl->cl_unsubscribe(topic);
        return (res == 0 ? MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS : MQTT_CLIENT_ERROR::MQTT_ERROR_FAILED);
    }

    virtual const MQTT_CLIENT_ERROR unsubscribe_all(){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__));

    }

    /*
    *
    */
    const std::shared_ptr<MqttObject>& get(){
        return m_messages->get();
    }

    /*
    *
    */
    const int put(const std::shared_ptr<MqttObject>& item){
        return m_mqttCl->cl_publish(item->topic(), item->payload());
    }

    /*
    *
    */
    const int disconnect(){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " State: " + (m_mqttCl ? std::to_string(m_mqttCl->is_connected()) : "No Client"));

        if(m_mqttCl && m_mqttCl->is_connected()){
            logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " MQTT client connected. Disconecting" );
            return m_mqttCl->cl_disconnect();
        }

        return MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS;
    }

    /*
    *
    */
    void on_client(mqtt::MQTT_CLIENT_STATE state, mqtt::MQTT_CLIENT_ERROR code){

    }

    /*
    *
    */
    const std::string get_version() const {
        return m_mqttCl->cl_get_version();
    }

    /*
    *
    */
    virtual bool start() override {
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started");
        return piutils::Threaded::start<mqtt::MqttClient<T>>(this);
    }

    /*
    * For Threaded
    */
    static void worker(MqttClient<T>* owner){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker started.");

        owner->connect();

        auto fn = [owner]{return (owner->is_stop_signal() || (!owner->is_empty() && owner->is_connected()));};
        while(!owner->is_stop_signal()){

            {
                std::unique_lock<std::mutex> lk(owner->cv_m);
                owner->cv.wait(lk, fn);
            }

            logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Message detected");

            while(owner->is_connected() && !owner->is_empty() && !owner->is_stop_signal()){

                //Publish message here
                const std::shared_ptr<MqttObject>& item = owner->get();
                owner->put(item);

                logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Message sent ");
            }
        }

        owner->disconnect();

        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker finished.");
    }

    /*
    *
    */
    virtual void stop() override{
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started.");
        //clear the queue
        m_messages->empty();
        //disconnect client
        disconnect();
        //stop working thread - do not try to send nothing
        piutils::Threaded::stop();
    }

    /*
    *
    */
    const bool is_empty() const {
        return m_messages->is_empty();
    }

private:
    MqttServerInfo m_conf;  //server configuration
    std::shared_ptr<T> m_mqttCl; //client implementation

    std::shared_ptr<piutils::circbuff::CircularBuffer<std::shared_ptr<MqttObject>>> m_messages;
    int m_max_size;
};

} //end namespace mqtt

#endif
