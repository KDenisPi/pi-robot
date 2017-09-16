/*
 * AnalogDataReceiverItf.h
 * Analog Data Receiver Interface
 *
 *  Created on: Sep 10, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_AnalogDataReceiverItf_H_
#define PI_LIBRARY_AnalogDataReceiverItf_H_

namespace pirobot {
namespace analogdata {

class AnalogDataReceiverItf {
public:
    AnalogDataReceiverItf() {}
    virtual ~AnalogDataReceiverItf() {}

    /*
    * Provide callback for receiving data from analog provider
    * Free this call as quick as you can otherwise you will block next data reading 
    */
    virtual void data(const unsigned short) = 0;

    virtual const std::string pname() const = 0;
};

}
}
#endif