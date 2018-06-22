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

namespace weather {

class StInitialization : public smachine::state::State {
public:
    StInitialization(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StInitialization") {
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
                std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_LCD_ON));
                EVENT(event);
            }
            else if(duration > 1 && duration <= 4){
                std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_SHOW_IP));
                EVENT(event);
            }
            else if(duration >= 5){
                std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_SHOW_IP));
                EVENT(event);
            }
        }
        else if(name == "btn_2"){
            if(duration >= 5){
                std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_FINISH));
                EVENT(event);
            }
        }
    }

    //
    // Detect IP address
    //
    void detect_ip_address(){
        piutils::netinfo::NetInfo netInfo;
        netInfo.load_ip_list();

        auto ctxt = get_env<weather::Context>();
        std::string ip4_address = ctxt->ip4_address;
        std::string ip6_address = ctxt->ip6_address;

        ctxt->ip4_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V4);
        ctxt->ip6_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V6);

        //
        //If IP address was changed or just detected - generate EVENT
        //
        if((!ip4_address.empty() && ctxt->ip4_address != ip4_address) || (!ip6_address.empty() && ctxt->ip6_address != ip6_address)){
            logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + " IP address changed IP4: " + ctxt->ip4_address + " IP6: " + ctxt->ip6_address);

            std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_SHOW_IP));
            EVENT(event);
        }
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
