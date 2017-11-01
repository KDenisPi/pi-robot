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

#include <logger.h>
#include "MqqtDefines.h"
#include "MqqtClientItf.h"

#include "Threaded.h"
#include "CircularBuffer.h"

namespace mqqt {


const char TAG_CL[] = "mqqtcl";

using pub_info = std::pair<int,std::pair<std::string, std::string>>;    

/*
* MQQT client implementation
*/
template <class T>
class MqqtClient : public piutils::Threaded{
public:
    /*
    *
    */
    MqqtClient<T>(const MqqtServerInfo& conf) : 
        m_mid(0),
        m_conf(conf),
        m_max_size(100)
    {
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Client ID: " + (NULL == m_conf.clientid() ? std::string("NotDefined") : m_conf.clientid()));
        m_mqqtCl = std::shared_ptr<T>(new T(m_conf.clientid()));
        m_mqqtCl->owner_notification = std::bind(&MqqtClient::on_client, this, std::placeholders::_1, std::placeholders::_2);
        
        m_messages = std::shared_ptr<piutils::circbuff::CircularBuffer<std::pair<std::string, std::string>>>(
                new piutils::circbuff::CircularBuffer<std::pair<std::string, std::string>>(m_max_size));
        
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Version: " + get_version());   
    }
    
    /*
    *
    */
    virtual ~MqqtClient(){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__));
        if(m_mqqtCl){
            m_mqqtCl->cl_disconnect();
        }
        m_mqqtCl.reset();
    }
    
    /*
    *
    */
    const int connect(){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__));
        return m_mqqtCl->cl_connect(m_conf);
    }

    /*
    * Temporal: Wait for processing.
    */
    void wait(){
        piutils::Threaded::stop(false);
    }


    const int get_next_mid(){
        return ++m_mid;
    }

    /*
    *
    */
    const MQQT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload){
        //do nothing if client stopped already 
        if(is_stop_signal())
            return m_mid;

        //std::lock_guard<std::mutex> lk(cv_m);
        auto pub_item = std::make_pair<get_next_mid(), std::make_pair<topic,payload>>;
        m_messages->put(pub_item);

        return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

    /*
    *
    */
    const std::shared_ptr<pub_info> get(){
        return m_messages->get();
    }

    /*
    *
    */
    const int put(pub_info& item){
        auto pub_item = item.second;
        return m_mqqtCl->cl_publish(item.first, pub_item.first, pub_item.second);
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
    bool start(){
        logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started");
        //return piutils::Threaded::start<MqqtClient<T>>(this);
        return true;
    }
    
    /*
    * For Threaded 
    */
    static void worker(MqqtClient* owner){
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker started.");
        
        auto fn = [owner]{return (owner->is_stop_signal() || !owner->is_empty());};        
        while(!owner->is_stop_signal()){

            {
                std::unique_lock<std::mutex> lk(owner->cv_m);
                owner->cv.wait(lk, fn);
            }

            while(!owner->is_empty() && !owner->is_stop_signal()){
                auto item = owner->get();

                //Publish message here
                owner->put(item);
            }
        }
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker finished.");
    }

    /*
    *
    */
    void stop() {
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
    int m_mid;  // message ID
    MqqtServerInfo m_conf;  //server configuration
    std::shared_ptr<T> m_mqqtCl; //client implementation

    std::recursive_mutex cv_m;
    std::condition_variable cv;
    
    std::shared_ptr<piutils::circbuff::CircularBuffer<std::pair<std::string, std::string>>> m_messages;
    int m_max_size;  
};

} //end namespace mqqt

#endif
