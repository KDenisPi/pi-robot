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

namespace mqqt {


const char TAG_CL[] = "mqqtcl";

using pub_info = std::pair<int,std::pair<std::string, std::string>>;    

/*
* MQQT client implementation
*/
template <class T>
class MqqtClient : public piutils::Threaded{
public:
    MqqtClient<T>(const MqqtServerInfo& conf) : 
        m_mid(0),
        m_conf(conf),
        m_max_size(100)
    {
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Client ID: " + (NULL == m_conf.clientid() ? std::string("NotDefined") : m_conf.clientid()));
        m_mqqtCl = std::shared_ptr<T>(new T(m_conf.clientid()));
        m_mqqtCl->owner_notification = std::bind(&MqqtClient::on_client, this, std::placeholders::_1, std::placeholders::_2);
        logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Version: " + get_version());   
    }
    
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
    *
    */
    const MQQT_CLIENT_ERROR publish(const std::string& topic, const std::string& payload){
        //do nothing if client stopped already 
        if(is_stop_signal())
            return m_mid;

	    mutex_sm.lock();
        if(m_messages.size() < m_max_size){
            ++m_mid;
            auto pub_item = std::make_pair<m_mid, std::make_pair<topic,payload>>;
            m_messages.push(pub_item);
        }
        mutex_sm.unlock();        
        return MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

    /*
    *
    */
    const std::shared_ptr<pub_info> get(){
	    mutex_sm.lock();
        std::shared_ptr<pub_info> item = m_messages.front();
        m_messages.pop();
        mutex_sm.unlock();
        return item;
    }

    const int put(const pub_info& item){
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
    * For Threaded 
    */
	static void* worker(void* p){
    	logger::log(logger::LLOG::NECECCARY, TAG_CL, std::string(__func__) + " Worker started.");

    	MqqtClient* owner = static_cast<MqqtClient*>(p);
        for(;;){
            if(owner->is_stop_signal())
                break;

            while(!owner->is_empty()){
                auto item = owner->get();

            }
    		delay(owner->get_loopDelay());
        }

    }

    /*
    *
    */
	void stop() {
    	logger::log(logger::LLOG::DEBUG, TAG_CL, std::string(__func__) + " Started.");
        //stop working thread - do not try to send nothing
	    piutils::Threaded::stop();
        //disconnect client
        disconnect();
        //clear the queue
        while(!m_messages.empty()){m_messages.pop();}
    }

    /*
    *
    */
    const bool is_empty() const {
        return m_messages.empty();
    }

private:
    int m_mid;  // message ID
    MqqtServerInfo m_conf;  //server configuration
    std::shared_ptr<T> m_mqqtCl; //client implementation

	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<std::pair<std::string, std::string>>> m_messages;
    int m_max_size;  
    
};

} //end namespace mqqt

#endif