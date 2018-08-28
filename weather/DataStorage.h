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

#include "MosquittoClient.h"
#include "MqqtClient.h"
#include "CircularBuffer.h"

#include "fstorage.h"
#include "measurement.h"

#ifdef USE_SQL_STORAGE
#include "pisqlite.h"
#endif

namespace weather{
namespace data {

/*
* Data storage interface
*/
class DataStorage {
    public:
    DataStorage() {}
    virtual ~DataStorage() {}

    virtual void stop() = 0;
    virtual bool write(Measurement& data) = 0;
};

/*
* Implementation of file based data storage
*/
class FileStorage : public DataStorage, public piutils::fstor::FStorage {
public:
    FileStorage() {
        set_first_line("date,humidity,temperature,pressure,luximity,co,tvoc,altitude\n");
    }

    virtual ~FileStorage() {}

    bool start(const std::string fstor_path, const bool local_time) {
        int res = initilize(fstor_path, local_time);
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
    /*
    *
    */
    MqqtStorage() {
            int res = mosqpp::lib_init();
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQQT library intialized Res: " + std::to_string(res));
    }

    /*
    *
    */
    virtual ~MqqtStorage() {
          mosqpp::lib_cleanup();
    }

    /*
    *
    */
    bool start(const mqqt::MqqtServerInfo mqqt_conf) {

        if(mqqt_conf.is_enable()){
            return false;
        }

        m_mqqt = std::shared_ptr<mqqt::MqqtItf>(new mqqt::MqqtClient<mqqt::MosquittoClient>(mqqt_conf));
        m_mqqt_active = m_mqqt->start();
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQQT configuration loaded Active: " + std::to_string(m_mqqt_active));

        return m_mqqt_active;
    }

    /*
    *
    */
    virtual void stop() override{
        m_mqqt->stop();
    }

    /*
    *
    */
    virtual bool write(Measurement& meas) override {
        MData data;
        meas.get_data(data);

        const char* sdata = data.to_string();
        mqqt::MQQT_CLIENT_ERROR res = mqqt_publish(_topic, strlen(sdata), sdata);

        return (res == mqqt::MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS);
    }

    /*
    *
    */
    const bool is_mqqt() const {
        return m_mqqt_active;
    }

private:
    virtual const mqqt::MQQT_CLIENT_ERROR mqqt_publish(const std::string& topic, const std::string& payload) {
        if(is_mqqt())
            return m_mqqt->publish(topic, payload);

        return mqqt::MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }

    virtual const mqqt::MQQT_CLIENT_ERROR mqqt_publish(const std::string& topic, const int payloadsize, const void* payload) {
        if(is_mqqt())
            return m_mqqt->publish(topic, payloadsize, payload);

        return mqqt::MQQT_CLIENT_ERROR::MQQT_ERROR_SUCCESS;
    }


    //MQQT support flag
    bool m_mqqt_active = false;
    std::shared_ptr<mqqt::MqqtItf> m_mqqt;

    std::string _topic = "weather";

};

#ifdef USE_SQL_STORAGE

/*
* Implementation of SQL based data storage
*/
class SqlStorage : public DataStorage, pisql::Sqlite {
public:
    SqlStorage() {}
    virtual ~SqlStorage() {}

    bool start(const std::string db_name) {
        return this->connect(db_name);
    }

    virtual void stop() override{
        this->disconnect();
    }

    virtual bool write(Measurement& meas) override{
        MData data;
        sqlite3_stmt *stmt = nullptr;
        int res = sqlite3_prepare_v2(db(),
            "INSERT INTO measurement(mdate, hum, temp, pressure, lux, co2, tvoc, altitude) values(?1,?2,?3,?4,?5,?6,?7,?8)", -1, &stmt, NULL);

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
#endif

} //namespace data
} //namespace weather
#endif