/*
 * context.h
 *
 *  Created on: Marh 16, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_CONTEXT_H_
#define WEATHER_CONTEXT_H_

#include "Environment.h"
#include "lcdstrings.h"
#include "defines.h"
#include "logger.h"
#include "DataStorage.h"
#include "smallthings.h"
#include "measurement.h"

namespace weather {

class Context : public smachine::Environment {
public:
    Context() : version("0.9"), ip4_address(""),
        ip6_address(""), _CO2_level(0), _TVOC_level(0) {
    }

    virtual ~Context() noexcept {}

    // Version
    std::string version;

    //IP addressed
    std::string ip4_address;
    std::string ip6_address;

    /*
    * UI language
    */
    std::string _language = "en";
    const std::string get_language() const {
        return _language;
    }

    /*
    * Database filename and location
    */
    std::string _db_name = _data_path + "/weather.db";
    std::string _data_list_file = _data_path + "/datafiles.csv";

    // Measurement data
    Measurement data;

    int light_off_on_diff = 600; //difference between light OFF and lights ON
    int light_low_level = 10;

    const bool show_temperature_in_celcius() const {
        return _temp_C;
    }

    //Update CO2 & TVOC levels
    void update_CO2_TVOC_levels(){
        int i;
        for(i = 0; i < 5; i++){
            if(data.spg30_co2 <_co2_levels[i]){
                _CO2_level = i;
                break;
            }
        }

        for(i = 0; i < 5; i++){
            if(data.spg30_tvoc <_tvoc_levels[i]){
                _TVOC_level = i;
                break;
            }
        }
        logger::log(logger::LLOG::DEBUG, "Ctxt", std::string(__func__) + "Levels CO2: " + std::to_string(_CO2_level) +
            " TVOC: " + std::to_string(_TVOC_level));
    }

    //
    const int get_CO2_level() const {
        return _CO2_level;
    }
    //
    const std::string& get_level_label(const int level, std::string& language) const {

        if(language == "ru" ){
            if(level<2) return get_str(StrID::Good_ru);
            if(level==2) return get_str(StrID::Moderate_ru);
            if(level==3) return get_str(StrID::Poor_ru);
            return get_str(StrID::Dangerous_ru);
        }

        if(level<2) return get_str(StrID::Good);
        if(level==2) return get_str(StrID::Moderate);
        if(level==3) return get_str(StrID::Poor);
        return get_str(StrID::Dangerous);
    }

    //
    const int get_TVOC_level() const {
        return _TVOC_level;
    }

    //
    //Timer intervals
    //
    int measure_check_interval = MEASURE_INTERVAL;
    int measure_show_interval = MEASURE_SHOW_INTERVAL;
    int measure_write_interval = MEASURE_WRITE_INTERVAL;
    int measure_light_interval = MEASURE_LIGHT_INTERVAL;
    int ip_check_interval = IP_CHECK_INTERVAL;

    //
    // Load/Save initial context data from/to file
    //
    // The most useful for restoring previous base values for SPG30 CO2 and TVOC
    //
    bool load_initial_data(const std::string& filename);
    bool save_initial_data(const std::string& filename);

    //Return string for UI interface
    const std::string& get_str(const int id) const {
        return _strs.get(id);
    }

    //
    // Is LCD switched On now
    //
    const bool is_lcd() const {
        return _lcd_on;
    }

    //
    // Save LCD On/Off state
    //
    void set_lcd(const bool lcd_on_off){
        _lcd_on = lcd_on_off;
    }

    /*
    * Prepare output string for measurement results
    */
    const std::string measure_view(const int line=0){
        char buff[512];
        memset(buff, 0x00, sizeof(buff));
        std::string lang = "en";

        if(line == 0){
            const int co2_level = get_CO2_level();
            const std::string co2 = get_co2_tvoc(data.spg30_co2);

            if(co2_level < 4){
                float temp = data.temp();
                sprintf(buff, get_str(StrID::Line1).c_str(), temp,
                    (show_temperature_in_celcius() ? 'C' : 'F'), co2.c_str());
            }
            else{
                const std::string co2_label = get_level_label(co2_level, lang);
                sprintf(buff, "CO2 %s!", co2.c_str());
            }
        }
        else if(line == 1){
            const int tvoc_level = get_TVOC_level();

            if(tvoc_level < 4){
                sprintf(buff, get_str(StrID::Line2).c_str(), data.si7021_humidity, '%', data.bmp280_pressure);
            }
            else{
                const std::string tvoc_label = get_level_label(tvoc_level, lang);
                const std::string tvoc = get_co2_tvoc(data.spg30_tvoc);
                sprintf(buff, "TVOC %s!", tvoc.c_str());
            }
        }

        std::string result(buff);
        return result;
    }

    /*
    *
    */
    const std::string get_co2_tvoc(const uint16_t value){
        char buff[32];
        if(value < 1000)
            sprintf(buff, "%d", value);
        else{
            float val = value/1000.0;
            sprintf(buff, "%.2fK", val);
        }

        std::string result(buff);
        return result;
    }

    /*
    * Initialize file base storage
    */
    void init_file_storage() {
        if(use_file_storage()){
            _fstorage = std::make_shared<weather::data::FileStorage>();
            std::string fline = "file,fpath\n";
            _fstorage->prepare_data_files_list(_fstor_path, _data_path, _data_list_file, fline);
        }
    }

    void stop_file_storage(){
        if(use_file_storage() && _fstorage){
            _fstorage->stop();
        }
    }

    const bool start_file_storage(){
        if(use_file_storage() && _fstorage){
            return _fstorage->start(_fstor_path, _fstor_local_time);
        }

        return false;
    }

    /*
    * Initialize MQTT based storage
    */
   void init_mqtt_storage() {
       if(use_mqtt_storage()){
           _mqttstorage = std::make_shared<weather::data::MqttStorage>("pirobot/sensors");
       }
   }

    void stop_mqtt_storage(){
        if(use_mqtt_storage() && _mqttstorage){
            _mqttstorage->stop();
        }
    }

    const bool  start_mqtt_storage(){
        if(use_mqtt_storage() && _mqttstorage){
            return _mqttstorage->start(_mqtt_conf);
        }

        return false;
    }

    const bool use_file_storage() const {
        return _use_file_storage;
    }

    const bool use_mqtt_storage() const {
        return _use_mqtt_storage;
    }

    const bool use_sql_storage() const {
        return _use_sql_storage;
    }

    /*
    * Write measument results
    */
    const bool write_measurement(const Measurement& meas){
        bool result = true;
        if(use_file_storage() && _fstorage){
            result = _fstorage->write(meas);
        }
        if(use_mqtt_storage() && _mqttstorage){
            result &= _mqttstorage->write(meas);
        }

        return result;
    }

    bool _use_file_storage = true;
    bool _use_mqtt_storage = false;
    bool _use_sql_storage = false;

    // File based data storage
    std::shared_ptr<weather::data::FileStorage> _fstorage;
    std::shared_ptr<weather::data::MqttStorage> _mqttstorage;

#ifdef USE_SQL_STORAGE
    std::shared_ptr<weather::data::SqlStorage> _sqlstorage;
#endif

private:
    LcdStrings _strs;

    //What should be used for temperature showing
    // true - celcius, false - fahrenheit
    bool _temp_C = true;

    //if LCD switched ON
    bool _lcd_on = true;

    //Concetration CO2 and/or TVOC
    int _CO2_level;
    int _TVOC_level;

    // Measurement interval for SPG30
    //  TVOC:  0 – 60'000 ppb
    //  CO2eq: 0 – 60'000 ppm
    //
    // ----- TVOC ----
    // Level        Hygienic                    TVOC [ppb]
    // 5  Unhealty  Situation not acceptable    2200 – 5500
    // 4  Poor      Major objections            660 – 2200
    // 3  Moderate  Some objections             220 – 660
    // 2  Good      No relevant objections      65 – 220
    // 1  Excellent No objections               0 – 65

    // ----- CO2 ----
    // Level
    // 1  250-350ppm	    Normal background concentration in outdoor ambient air
    // 2  350-1,000ppm	    Concentrations typical of occupied indoor spaces with good air exchange
    // 3  1,000-2,000ppm	Complaints of drowsiness and poor air.
    // 4  2,000-5,000 ppm	Headaches, sleepiness and stagnant, stale, stuffy air.
    //                      Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
    //    5,000	            Workplace exposure limit (as 8-hour TWA) in most jurisdictions.
    // 5  >40,000 ppm	    Exposure may lead to serious oxygen deprivation resulting in permanent brain damage, coma, even death.

    uint16_t _tvoc_levels[5] = {65, 220, 660, 2200, 60000};
    uint16_t _co2_levels[5] = {350, 1000, 2000, 5000, 60000};
};

}

#endif /* WEATHER_CONTEXT_H_ */
