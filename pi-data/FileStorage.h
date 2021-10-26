/*
 * FileStorage.h
 *
 *  File based data storage results
 *
 *  Created on: Feb 14, 2020
 *      Author: Denis Kudia
 */

#ifndef PIDATA_FILE_STORAGE_H_
#define PIDATA_FILE_STORAGE_H_

#include "DataStorage.h"
#include "fstorage.h"

namespace pidata{
namespace filestorage {

template<typename T>
class FileStorage : public pidata::datastorage::DataStorage<T>, public piutils::fstor::FStorage {
public:
    FileStorage() {
        set_first_line("date,humidity,temperature,pressure,luximity,co,tvoc,altitude\n");
    }

    virtual ~FileStorage() {}

    bool start(const std::string fstor_path, const bool local_time) {
        int res = initilize(fstor_path, local_time);
        return datastorage::DataStorage<T>::set_run((res == 0));
    }

    virtual void stop() override {
        FStorage::stop();
    }

    virtual const uint32_t get_status() override {
        return (datastorage::DataStorage<T>::is_run() && is_fd() ? pidata::datastorage::s_UP : pidata::datastorage::s_DOWN);
    }
};

} //namespace filestorage
} //namespace pidata

#endif