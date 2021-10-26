/*
 * AnalogDataReceiverItf.h
 * Analog Data Receiver Interface
 *
 *  Created on: Sep 10, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_AnalogDataReceiverItf_H_
#define PI_LIBRARY_AnalogDataReceiverItf_H_

#include <mutex>

namespace pirobot {
namespace analogdata {

class AnalogDataReceiverItf {
public:
    AnalogDataReceiverItf(const std::string& name, const int idx) : _name(name), m_idx(idx), _data(0) {}
    virtual ~AnalogDataReceiverItf() {}

    /*
    * Save received data
    * Free this call as quick as you can otherwise you will block next data reading
    */
    virtual void data(const uint16_t data){
        logger::log(logger::LLOG::DEBUG, "ARecvItf", std::string(__func__) + " Name: " + pname() + " Data: " + std::to_string(data));
        std::lock_guard<std::mutex> lock(_mt_data);
        _data = data;
    }

    virtual const uint16_t get_data(){
        std::lock_guard<std::mutex> lock(_mt_data);
        return _data;
    }

    virtual const float get_value() {
        return 0.0;
    }

    virtual const std::string pname() const {
        return _name;
    }

    const bool is_active() const{
        return m_active;
    }

    virtual void activate(){
        m_active = true;
    }

    void deactivate(){
        m_active = false;
    }

    const int get_idx() const {
        return m_idx;
    }

    const void set_idx(const int idx){
        m_idx = idx;
    }

    /**
     * Functiona called before and after measurement. Implement special functionality.
     */
    virtual bool before(){
        return true;
    }

    virtual bool after(){
        return true;
    }

    /*
    *
    */
    const std::string printConfig(){
        std::string result = pname() + " Pin: " + std::to_string(m_idx) + " Active: " + std::to_string(m_active);
        return result;
    }

private:
    bool m_active;  //If we should send data to this meter
    int m_idx;      //Analog Input assigned for this device
    std::string _name;

    uint16_t _data;
    std::mutex _mt_data;

};

}
}
#endif