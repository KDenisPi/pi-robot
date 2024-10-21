/*
 * StNoHardware.h
 *
 * @brief State used for test purposes
 *
 *  Created on: June 29, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STNOHARDWARE_H_
#define WEATHER_STNOHARDWARE_H_

#include "StateMachine.h"
#include "defines.h"

#include "networkinfo.h"

namespace weather {

class StNoHardware : public smachine::state::State {
public:
    StNoHardware() : smachine::state::State("StNoHardware") {}
    virtual ~StNoHardware(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

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