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
    logger::log(logger::LLOG::DEBUG, "BMP280", std::string(__func__) + " Started ");
    pi2c p_pi2c = std::make_shared<pirobot::i2c::I2C>();
    bmp280 p_bmp280 = std::make_shared<pirobot::item::Bmp280>("Bmp280", p_pi2c);

    const uint8_t status = p_bmp280->get_status();
    std::cout << "Status: " << status << std::endl;
    float pressure, temp, altitude;

    for(int i=0; i<5; i++){
      if(p_bmp280->get_results(pressure, temp, altitude)){
          std::cout << "Results: Pressure: " << pressure << " Temp (C): " << temp << " Altitude: " << altitude << std::endl;
      }
      else{
          std::cout << "Could not get results" << std::endl;
          success = false;
          break;
      }
      sleep(2);
    }
    std::cout << "Release Bmp280 objects " << std::endl;
    p_bmp280.reset();
    std::cout << "Release Provider objects " << std::endl;
    p_pi2c.reset();

    //std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));

}
