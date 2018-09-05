/*
 * context.cpp
 *
 *  Created on: Marh 16, 2018
 *      Author: Denis Kudia
 */

#include "context.h"
#include "logger.h"
#include "JsonHelper.h"

namespace weather {

/*
* Load initial data from file
*/
bool Context::load_initial_data(const std::string& filename){

    std::ifstream ijson(filename);
    if (!ijson.is_open()){
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Could not open configuraion file: " + filename);
        return false;
    }

    try{
        logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Parse JSON from: " + filename);

        jsoncons::json conf = jsoncons::json::parse(ijson);
        version = jsonhelper::get_attr<std::string>(conf, "version", "0.9");

        //
        // This values used for Spg30 calibration
        //
        data.spg30_base_co2 = jsonhelper::get_attr<uint16_t>(conf, "base_co2", 0);
        data.spg30_base_tvoc = jsonhelper::get_attr<uint16_t>(conf, "base_tvoc", 0);

    }
    catch(jsoncons::parse_error& perr){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid configuration " +
            perr.what() + " Line: " + std::to_string(perr.line_number()) + " Column: " + std::to_string(perr.column_number()));
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

    //check if previous file exist and
    jsoncons::json conf = jsoncons::json::object{
        {"version", version},
        {"base_co2", std::to_string(data.spg30_base_co2)},
        {"base_tvoc", std::to_string(data.spg30_base_tvoc)}
    };

    jsoncons::json sgp30;
    sgp30.insert_or_assign("co2", std::to_string(data.spg30_co2));
    sgp30.insert_or_assign("tvoc", std::to_string(data.spg30_tvoc));

    jsoncons::json si7021;
    si7021.insert_or_assign("humidity", std::to_string(data.si7021_humidity));
    si7021.insert_or_assign("temperature", std::to_string(data.si7021_temperature));
    si7021.insert_or_assign("absolute_humidity", std::to_string(data.si7021_abs_humidity));

    jsoncons::json bmp280;
    bmp280.insert_or_assign("pressure", std::to_string(data.bmp280_pressure));
    bmp280.insert_or_assign("temperature", std::to_string(data.bmp280_temperature));
    bmp280.insert_or_assign("altitude", std::to_string(data.bmp280_altitude));

    jsoncons::json tsl2651;
    tsl2651.insert_or_assign("luminosity", std::to_string(data.tsl2651_lux));

    conf["sgp30"] = std::move(sgp30);
    conf["si7021"] = std::move(si7021);
    conf["bmp280"] = std::move(bmp280);
    conf["tsl2651"] = std::move(tsl2651);

    std::ofstream os_file(filename);
    os_file << conf;

    os_file.close();
}

}