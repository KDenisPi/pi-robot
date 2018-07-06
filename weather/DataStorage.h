/*
 * DataStorage.h
 *
 *  Interface for saving measurement results
 *
 *  Created on: Jul 03, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_DATA_STORAGE_H_
#define WEATHER_DATA_STORAGE_H_

#include "fstorage.h"
#include "measurement.h"
#include "context.h"
#include "pisqlite.h"

namespace weather{
namespace data {

/*
* Data storage interface
*/
class DataStorage {
    public:
    DataStorage() {}
    virtual ~DataStorage() {}

    virtual bool start(const std::shared_ptr<Context> ctxt) = 0;
    virtual void stop() = 0;
    virtual bool write(Measurement& data) = 0;
};

/*
* Implementation of file based data storage
*/
class FileStorage : public DataStorage, piutils::fstor::FStorage {
public:
    FileStorage() {}
    virtual ~FileStorage() {}

    virtual bool start(const std::shared_ptr<Context> ctxt) override {
        int res = initilize(ctxt->_fstor_path, ctxt->_fstor_local_time);
        return (res == 0);
    }

    virtual void stop() override {
        FStorage::stop();
    }

    virtual bool write(Measurement& meas) override {
        MData data;
        meas.get_data(data);

        const char* sdata = data.to_string();
        int res = write_data(sdata, strlen(sdata));

        return (res == 0);
    }
};

/*
* Implementation of MQQT protocol based data storage
*/
class MqqtStorage : public DataStorage {
public:
    MqqtStorage() {}
    virtual ~MqqtStorage() {}

    virtual bool start(const std::shared_ptr<Context> ctxt) override {
        return true;
    }

    virtual void stop() override{
    }

    virtual bool write(Measurement& data) override {
        return true;
    }
};

/*
* Implementation of SQL based data storage
*/
class SqlStorage : public DataStorage, pisql::Sqlite {
public:
    SqlStorage() {}
    virtual ~SqlStorage() {}

    virtual bool start(const std::shared_ptr<Context> ctxt) override{
        return this->connect(ctxt->_db_name);
    }

    virtual void stop() override{
        this->disconnect();
    }

    virtual bool write(Measurement& meas) override{
        MData data;
        sqlite3_stmt *stmt = nullptr;
        int res = sqlite3_prepare_v2(db(), "INSERT INTO measurement(mdate, hum, temp, pressure, lux, co2, tvoc, altitude) values(?1,?2,?3,?4,?5,?6,?7,?8)", -1, &stmt, NULL);

        meas.get_data(data);
        res = sqlite3_bind_text(stmt, 1, data.dtime, strlen(data.dtime), SQLITE_STATIC);
        for(int i = 0; i < 7; i++){
            res = sqlite3_bind_int(stmt, i+2, data.data[i]);
        }

        res = sqlite3_step(stmt);
        if(res != SQLITE_DONE){

            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }
};


} //namespace data
} //namespace weather
#endif