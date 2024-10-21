/*
 * StInitialization.h
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STINITIALIZATION_H_
#define WEATHER_STINITIALIZATION_H_

#include "StateMachine.h"
#include "context.h"
#include "defines.h"
#include "Event.h"
#include "networkinfo.h"
#include "logger.h"
#include "send_email.h"

namespace weather {

class StInitialization : public smachine::state::State {
public:
    StInitialization() : smachine::state::State("StInitialization") {
    }

    virtual ~StInitialization(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;
    virtual void OnSubstateExit(const std::string substate_name) override;

private:
    std::chrono::time_point<std::chrono::system_clock> _btn1_down;
    std::chrono::time_point<std::chrono::system_clock> _btn2_down;
    std::chrono::time_point<std::chrono::system_clock> _btn1_up;
    std::chrono::time_point<std::chrono::system_clock> _btn2_up;

    //Genarate a new event on base how many time button was pressed
    void generate_button_press(const std::string& name, const int duration){
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + "  Name: " + name + " Duration: " + std::to_string(duration));

        if(duration <= 0)
            return;

        // Button 1
        // Short press (< 3 sec) - switch LCD On
        // Long press  (> 5 sec) - Show IP address
        //
        // Button 2
        // Long press Btn2 (> 5 sec) - finish
        //

        if( name == "btn_1"){
            if(duration <= 1){
                event_add(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_LCD_ON));
            }
            else if(duration > 1 && duration <= 3){
                event_add(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_SHOW_IP));
            }
            else if(duration >= 4){ //for test purpose only
                event_add(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_HIGH_LEVEL_ON));
            }
        }
        else if(name == "btn_2"){
            if(duration >= 5){
                event_add(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_FINISH));
            }
        }
    }

    //
    // Detect IP address
    //
    void detect_ip_address(){
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__));

        piutils::netinfo::NetInfo netInfo;
        netInfo.load_ip_list();

        auto ctxt = get_env<weather::Context>();
        std::string ip4_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V4);
        std::string ip6_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V6);

        //
        //If IP address was changed or just detected - generate EVENT
        //
        if( !ip4_address.empty() ){
            if((ctxt->ip4_address != ip4_address) ){
                logger::log(logger::LLOG::INFO, "StInit", std::string(__func__) + " IP address changed from: " + ctxt->ip4_address +
                        " [" + ctxt->ip6_address + "] to: " + ip4_address + "[" + ip6_address + "]");

                ctxt->ip4_address = ip4_address;
                ctxt->ip6_address = ip6_address;

                event_add(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_SHOW_IP));
                send_email_ip_changed(ctxt->ip4_address, ctxt->ip6_address);
            }
        }
    }

    void send_email_ip_changed(const std::string& ip_4, const std::string& ip_6){
        logger::log(logger::LLOG::INFO, "StInit", std::string(__func__) + " IPv6: " + ip_6 + " IPv4: " + ip_4);

        auto ctxt = get_env<weather::Context>();

        if(ctxt->_email_password.empty()){
            logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + " No email password. Do not try to send email");
            return;
        }

        piutils::email::SendEmail email(ctxt->_email_server, ctxt->_email_cert);

        email._user = ctxt->_email_user;
        email._password = ctxt->_email_password;
        email._postfix = "gmail.com"; //TODO change

        email._subject = "IP address changed to " + ip_4;
        email._body = "IP address changed to " + ip_4 + " (" + ip_6 + ")";
        email._from = ctxt->_email_from;
        email._to   = ctxt->_email_to;

        email.prepare_data();
        int result = email.send_email();

        logger::log(logger::LLOG::INFO, "StInit", std::string(__func__) + " To: " + ctxt->_email_to + " Result: " + std::to_string(result));
    }

    //
    // Detect IP address
    // First try to detect Wi-Fi, the second Ethernet
    //
    const std::string  detect_ip_address_by_type(piutils::netinfo::NetInfo* netInfo, const piutils::netinfo::IpType ip_type){
        std::list<piutils::netinfo::ItfInfo> itf_ip_wlan = netInfo->get_default_ip(ip_type, "wl");
        if(!itf_ip_wlan.empty()){
            return itf_ip_wlan.front().second;
        }
        else {
            std::list<piutils::netinfo::ItfInfo> itf_ip_eth = netInfo->get_default_ip(ip_type, "e");
            if(!itf_ip_eth.empty()){
                return itf_ip_eth.front().second;
            }
        }

        return "Not detected";
    }

};

}//weather namespace
#endif
