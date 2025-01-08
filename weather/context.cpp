/*
 * context.cpp
 *
 *  Created on: Marh 16, 2018
 *      Author: Denis Kudia
 */

#include "logger.h"
#include "context.h"
#include "cJSON_wrap.h"

namespace weather {

/*
* Load initial data from file
*/
bool Context::load_initial_data(const std::string& filename){

    const std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(filename));
    if(!fl->is_success()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + "Could not load file " + fl->get_name() + " Error: " + std::to_string(fl->get_err()));
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Parse JSON from: " + filename);
    std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>(fl);
    if(!cjson->is_succeess()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not parse JSON: " + cjson->get_error());
        return false;
    }

    try{
        version = cjson->get_attr_string_def(cjson->get(), "version", "1.0");

        //
        // This values used for Spg30 calibration
        //
        data.spg30_base_co2 = cjson->get_attr_def<uint16_t>(cjson->get(), "base_co2", 0);
        data.spg30_base_tvoc = cjson->get_attr_def<uint16_t>(cjson->get(), "base_tvoc", 0);

    }
    catch(std::runtime_error& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Initial configuration loaded successfully");
    return true;
}

/*
* Save current values to file
*/
bool Context::save_initial_data(const std::string& filename){

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Save configuration to JSON: " + filename);

    std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>();

    try{
        cjson->add_string(cjson->get(), "version", "1.00");
        cjson->add_number<uint16_t>(cjson->get(),"base_co2", data.spg30_base_co2);
        cjson->add_number<uint16_t>(cjson->get(),"base_tvoc", data.spg30_base_tvoc);

        auto sgp30 = cjson->add_object(cjson->get(), "sgp30");
        cjson->add_number<uint16_t>(sgp30,"co2", data.spg30_co2);
        cjson->add_number<uint16_t>(sgp30,"tvoc", data.spg30_tvoc);

        auto si7021 = cjson->add_object(cjson->get(), "si7021");
        cjson->add_number<float>(si7021,"humidity", data.si7021_humidity);
        cjson->add_number<float>(si7021,"temperature", data.si7021_temperature);
        cjson->add_number<float>(si7021,"absolute_humidity", data.si7021_abs_humidity);

        auto bmp280 = cjson->add_object(cjson->get(), "bmp280");
        cjson->add_number<float>(bmp280,"pressure", data.bmp280_pressure);
        cjson->add_number<float>(bmp280,"temperature", data.bmp280_temperature);
        cjson->add_number<float>(bmp280,"altitude", data.bmp280_altitude);

        auto tsl2651 = cjson->add_object(cjson->get(), "tsl2651");
        cjson->add_number<uint32_t>(tsl2651,"luminosity", data.tsl2651_lux);


        std::string json = cjson->print();
        std::ofstream os_file(filename);
        os_file << json;
        os_file.close();
    }
    catch(std::runtime_error& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
        return false;
    }

    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Configuration saved successfully");
    return true;
}

}