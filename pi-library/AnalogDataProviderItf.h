/*
 * AnalogDataProviderItf.h
 * Analog Data Provider Interface
 *
 *  Created on: Sep 10, 2017
 *      Author: Denis Kudia
 */

 #ifndef PI_LIBRARY_AnalogDataProviderItf_H_
 #define PI_LIBRARY_AnalogDataProviderItf_H_

 #include "AnalogDataReceiverItf.h"

namespace pirobot {
namespace analogdata {

class AnalogDataProviderItf {
public:
    AnalogDataProviderItf() {}
    virtual ~AnalogDataProviderItf() {}

    /*
    * Link data receiver and data provider
    */
    virtual bool register_data_receiver(const int input_idx, 
        const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> receiver) noexcept(false) = 0;

    virtual void activate_data_receiver(const int input_idx) = 0;
    virtual const std::string pname() const = 0;
};

}
}
#endif