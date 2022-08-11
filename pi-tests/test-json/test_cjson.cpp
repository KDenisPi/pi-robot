#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <limits>

#include "cJSON_wrap.h"

using namespace std;

using my_data = struct _data {
    // Si7021 - I2C Humidity and Temperature Sensor
    float si7021_humidity;
    float si7021_temperature;
    float si7021_abs_humidity;

    // SPG30 - I2C Sensurion Gas Platform
    uint16_t spg30_co2;
    uint16_t spg30_tvoc;
    uint16_t spg30_base_co2;
    uint16_t spg30_base_tvoc;

    // BMP280 - I2C Digital Presure Sensor
    float bmp280_pressure;
    float bmp280_temperature;
    float bmp280_altitude;

    // TSL2561 - I2C Light-to-Digital Converter
    uint32_t tsl2651_lux;
};


int main (int argc, char* argv[])
{
    bool result = true;
    my_data data = {0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0};

    if(argc==1){
        std::cout << "No configuration file" << std::endl;
        exit(EXIT_FAILURE);
    }

    /*
    Test file parsing
    */
    {
        const std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(argv[1]));
        if(!fl->is_success()){
            std::cout << "Could not load file " << fl->get_name() << " Error: " << fl->get_err() << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Loaded file " << fl->get_name() << " Size: " << fl->get_size() << std::endl;
        std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>(fl);

        try{
            std::string ver = cjson->get_attr_string(cjson->get(), std::string("version"));
            std::cout << "Version: " << ver << std::endl;
            bool real_w = cjson->get_attr<bool>(cjson->get(), std::string("real_world"));
            std::cout << "real_world: " << real_w << std::endl;

            auto providers = cjson->get_array(cjson->get(), std::string("providers"));
            auto provider = cjson->get_first(providers);
            while(provider){
                auto pr_type = cjson->get_attr_string(provider, std::string("type"));
                std::cout << "type: " << pr_type << std::endl;

                provider = cjson->get_next(provider);
            }

            auto gpios = cjson->get_array(cjson->get(), std::string("gpios"));
            auto gpio = cjson->get_first(gpios);
            while(gpio){
                auto pr_type = cjson->get_attr_string(gpio, std::string("name"));
                std::cout << "name: " << pr_type << std::endl;

                gpio = cjson->get_next(gpio);
            }
        }
        catch(std::runtime_error& exc){
            std::cout << "Error: "<< + exc.what() << std::endl;
            result = false;
        }
    }

    /*
    Test file creating
    */
    if(result){
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
            std::cout << "json: " << json << std::endl;

        }
        catch(std::runtime_error& exc){
            std::cout << "Error: "<< + exc.what() << std::endl;
            result = false;
        }

    }


    exit(EXIT_SUCCESS);
}
