/*
 * DataStorage.h
 *
 *  Interface for saving results
 *
 *  Created on: Feb 14, 2020
 *      Author: Denis Kudia
 */

#ifndef PIDATA_DATA_STORAGE_H_
#define PIDATA_DATA_STORAGE_H_

namespace pidata{
namespace datastorage {

/*
* Data storage interface
*/
template<typename T>
class DataStorage {
public:
    DataStorage() {}
    virtual ~DataStorage() {}

    static constexpr uint32_t s_DOWN        = 0;
    static constexpr uint32_t s_UP          = 1;
    static constexpr uint32_t s_CONNECTED   = 2;
    static constexpr uint32_t s_CONNCTING   = 4;

    virtual void stop() = 0;
    virtual bool write(const T& data) = 0;
    virtual const uint32_t get_status() = 0;

    virtual const bool is_run() {return _run;}
    virtual bool set_run(const bool run){
        _run = run;
        return _run;
    }

    /*
    * TODO: add notification function to UP level
    */

protected:
    bool _run = false;
};

}//namespace datastorage
}//namespace pidata

#endif