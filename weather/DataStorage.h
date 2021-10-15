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
#include "MqttClient.h"
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
* Implementation of mqtt protocol based data storage
*/
class MqttStorage : public DataStorage {
public:
    /*
    *
    */
    MqttStorage() {
            int res = mosqpp::lib_init();
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "mqtt library intialized Res: " + std::to_string(res));
    }

    /*
    *
    */
    virtual ~MqttStorage() {
          mosqpp::lib_cleanup();
    }

    /*
    *
    */
    bool start(const mqtt::MqttServerInfo mqtt_conf) {

        if(mqtt_conf.is_enable()){
            return false;
        }

        m_mqtt = std::shared_ptr<mqtt::MqttItf>(new mqtt::MqttClient<mqtt::MosquittoClient>(mqtt_conf));
        m_mqtt_active = m_mqtt->start();
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "mqtt configuration loaded Active: " + std::to_string(m_mqtt_active));

        return m_mqtt_active;
    }

    /*
    *
    */
    virtual void stop() override{
        m_mqtt->stop();
    }

    /*
    *
    */
    virtual bool write(Measurement& meas) override {
        MData data;
        meas.get_data(data);

        const char* sdata = data.to_string();
        mqtt::mqtt_CLIENT_ERROR res = mqtt_publish(_topic, strlen(sdata), sdata);

        return (res == mqtt::mqtt_CLIENT_ERROR::mqtt_ERROR_SUCCESS);
    }

    /*
    *
    */
    const bool is_mqtt() const {
        return m_mqtt_active;
    }

private:
    virtual const mqtt::mqtt_CLIENT_ERROR mqtt_publish(const std::string& topic, const std::string& payload) {
        if(is_mqtt())
            return m_mqtt->publish(topic, payload);

        return mqtt::mqtt_CLIENT_ERROR::mqtt_ERROR_SUCCESS;
    }

    virtual const mqtt::mqtt_CLIENT_ERROR mqtt_publish(const std::string& topic, const int payloadsize, const void* payload) {
        if(is_mqtt())
            return m_mqtt->publish(topic, payloadsize, payload);

        return mqtt::mqtt_CLIENT_ERROR::mqtt_ERROR_SUCCESS;
    }


    //mqtt support flag
    bool m_mqtt_active = false;
    std::shared_ptr<mqtt::MqttItf> m_mqtt;

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