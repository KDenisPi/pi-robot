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

//#include "CircularBuffer.h"

#include "FileStorage.h"
#include "MqttStorage.h"
#include "measurement.h"

#ifdef USE_SQL_STORAGE
#include "pisqlite.h"
#endif

namespace weather{
namespace data {

/*
* Implementation of file based data storage
*/
class FileStorage : public pidata::filestorage::FileStorage<weather::Measurement> {
public:
    FileStorage() {
        set_first_line("date,humidity,temperature,pressure,luximity,co,tvoc,altitude\n");
    }

    virtual ~FileStorage() {}

    virtual bool write(const Measurement& meas) override {
        MData data;
        meas.copy_data(data);

        const std::string sdata = data.to_string();
        int res = write_data(sdata.c_str(), sdata.length());

        return (res == 0);
    }
};

/*
* Implementation of MQTT protocol based data storage
*/
class MqttStorage : public pidata::mqttstorage::MqttStorage<weather::Measurement> {
public:

    MqttStorage() {
        set_topic("weather");
    }

    virtual ~MqttStorage() {
    }

    /*
    *
    */
    virtual bool write(const Measurement& meas) override {
        MData data;
        meas.copy_data(data);

        const std::string sdata = data.to_json();
        logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + "MQTT data: " + sdata);
        mqtt::MQTT_CLIENT_ERROR res = mqtt_publish(topic(), sdata);

        return (res == mqtt::MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS);
    }
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