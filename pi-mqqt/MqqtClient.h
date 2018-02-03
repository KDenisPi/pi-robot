/*
 * MqqtClient.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_CLIENT_H_
#define PI_MQQT_CLIENT_H_

#include <memory>
#include <functional>
#include <queue>

#include "logger.h"

#include "MqqtDefines.h"
#include "MqqtClientItf.h"
#include "mqqt_object.h"

#include "Threaded.h"
#include "CircularBuffer.h"

namespace mqqt {


const char TAG_CL[] = "mqqtcl";

using pub_info = std::pair<std::string,std::pair<int, std::string>>;    

class MqqtItf {
public:
    MqqtItf() {}
    virtual ~MqqtItf() {}

    virtual bool start() = 0;
    virtual void stop() = 0;

    virtual const MQQT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload) = 0;
    virtual const MQQT_CLIENT_ERROR publish(const std::string& topic, const int payloadsize, const void* payload) = 0;
};

/*
* MQQT client implementation
*/
template <class T>
class MqqtClient : public piutils::Threaded, public MqqtItf {
public:
    /*
    *
    */
    MqqtClient<T>(const MqqtServerInfo& conf) : 
        m_conf(conf),
        m_max_size(100)
    {
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + m_conf.to_string() );
        
        m_mqqtCl = std::shared_ptr<T>(new T(m_conf.clientid()));
        m_mqqtCl->owner_notification = std::bind(&MqqtClient::on_client, this, std::placeholders::_1, std::placeholders::_2);
        
        m_messages = std::shared_ptr<piutils::circbuff::CircularBuffer<MqqtObject*>>(new piutils::circbuff::CircularBuffer<MqqtObject*>(m_max_size));
        
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Version: " + get_version());   
    }
    
    /*
    *
    */
    virtual ~MqqtClient(){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started");
        if(m_mqqtCl){
            m_mqqtCl->cl_disconnect();
        }
        m_mqqtCl.reset();
    }
    
    /*
    *
    */
    const int connect(){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Started");
        return m_mqqtCl->cl_connect(m_conf);
    }

    const bool is_connected() const {
        return m_mqqtCl->is_connected();
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
    const MQQT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload) override {
        //do nothing if client stopped already 
        if(is_stop_signal())
            return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;

        m_messages->put(new MqqtObject(topic, payload));
        return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

    virtual const MQQT_CLIENT_ERROR publish(const std::string& topic, const int payloadsize, const void* payload) override {
        //do nothing if client stopped already 
        if(is_stop_signal())
            return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;

        m_messages->put(new MqqtObject(topic, payloadsize, payload));
        return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }


    /*
    *
    */
    const MqqtObject* get(){
        return m_messages->get();
    }

    /*
    *
    */
    const int put(const MqqtObject* item){
        int mid = 0;
        return m_mqqtCl->cl_publish(&mid, item->topic(), item->payloadsize(), item->payload().get());
    }

    /*
    *
    */
    const int disconnect(){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__));
        return m_mqqtCl->cl_disconnect();
    }

    /*
    *
    */
    void on_client(mqqt::MQQT_CLIENT_STATE state, mqqt::MQQT_CLIENT_ERROR code){
        
    }

    /*
    *
    */
    const std::string get_version() const {
        return m_mqqtCl->cl_get_version();
    }

    /*
    *
    */
    virtual bool start() override {
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started");
        return piutils::Threaded::start<mqqt::MqqtClient<T>>(this);
    }
    
    /*
    * For Threaded 
    */
    static void worker(MqqtClient<T>* owner){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker started.");
        
        owner->connect();

        auto fn = [owner]{return (owner->is_stop_signal() || (!owner->is_empty() && owner->is_connected()));};        
        while(!owner->is_stop_signal()){

            {
                std::unique_lock<std::mutex> lk(owner->cv_m);
                owner->cv.wait(lk, fn);
            }

            while(owner->is_connected() && !owner->is_empty() && !owner->is_stop_signal()){
                //Publish message here
                const MqqtObject* item = owner->get();
                owner->put(item);

                delete item;
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
    MqqtServerInfo m_conf;  //server configuration
    std::shared_ptr<T> m_mqqtCl; //client implementation

    std::shared_ptr<piutils::circbuff::CircularBuffer<MqqtObject*>> m_messages;
    int m_max_size;  
};

} //end namespace mqqt

#endif
