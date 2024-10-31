/*
 * StNoHardware.cpp
 *
 *  Created on: June 29, 2018
 *      Author: Denis Kudia
 */

#include "StNoHardware.h"
#include "context.h"

#include "fstorage.h"

namespace weather {

void StNoHardware::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    piutils::netinfo::NetInfo netInfo;
    netInfo.load_ip_list();

    auto ctxt = get_env<weather::Context>();
    ctxt->ip4_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V4);
    ctxt->ip6_address = detect_ip_address_by_type(&netInfo, piutils::netinfo::IpType::IP_V6);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " IP4: " + ctxt->ip4_address);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " IP6: " + ctxt->ip6_address);

#ifdef USE_FILE_STORAGE
    std::string fline = "file,fpath\n";
    ctxt->_fstorage.prepare_data_files_list(ctxt->_fstor_path, ctxt->_data_path, ctxt->_data_list_file, fline);
#endif

    init_timer(TIMER_FINISH_ROBOT, 55, 0, false);
}

bool StNoHardware::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            finish();
            return true;
        }
    }

    return false;
}

bool StNoHardware::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Event: " + event->to_string());
    return false;
}

}//namespace weather
