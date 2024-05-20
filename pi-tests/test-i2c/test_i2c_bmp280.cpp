#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "bmp280.h"

using pi2c = std::shared_ptr<pirobot::i2c::I2C>;
using bmp280 = std::shared_ptr<pirobot::item::Bmp280>;

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    //std::cout << "Create provider" << std::endl;
    pi2c p_pi2c = std::make_shared<pirobot::i2c::I2C>();
    bmp280 p_bmp280 = std::make_shared<pirobot::item::Bmp280>("Bmp280", p_pi2c);

    const uint8_t status = p_bmp280->get_status();
    float pressure, temp, altitude;

    if(p_bmp280->get_results(pressure, temp, altitude)){
        std::cout << "Results: Pressure: " << pressure << " Temp (C): " << temp << " Altitude: " << altitude << std::endl;
    }
    else{
        std::cout << "Could not get results" << std::endl;
        success = false;
    }

    std::cout << "Release Bmp280 objects " << std::endl;
    p_bmp280.reset();
    std::cout << "Release Provider objects " << std::endl;
    p_pi2c.reset();

    //std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));

}
