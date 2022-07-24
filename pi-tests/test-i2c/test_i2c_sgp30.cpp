#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "sgp30.h"
#include "Si7021.h"


using pi2c = std::shared_ptr<pirobot::i2c::I2C>;
using psgp30 = std::shared_ptr<pirobot::item::Sgp30>;

/*
* I2C SGP30 test.
* Precondition: Connect SGP30 sensor
* 1. Connect to default I2C address
* 2. Get value during some hours and save results
*/


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    uint32_t lux = 0;
    int interval = 10;
    //std::cout << "Started. Parameters: tempearture humidity test_seconds result_file." << std::endl
    //    << "Otherwise will be used 20.5 0.50 60 /var/log/pi-robot/sgp30.csv" << std::endl;

    float temp = (argc < 3 ? 20.5 : std::atof(argv[1]));
    float humd = (argc < 3 ? 50.0 : std::atof(argv[2]));
    float abs_humd = pirobot::item::Si7021::get_absolute_humidity(temp, humd);
    int seconds = (argc >= 4 ? std::atoi(argv[3]) : 60);

    //std::cout << "Incoming parameters T:" << temp << " Humidity: " << humd << " Abs. Humidity: " << abs_humd << " Measurement time (sec): " << seconds << std::endl;

    //std::cout << "Create provider" << std::endl;
    pi2c p_pi2c = std::make_shared<pirobot::i2c::I2C>();

    psgp30 p_psgp30 = std::make_shared<pirobot::item::Sgp30>("SGP30", p_pi2c);
    p_psgp30->set_humidity(abs_humd);
    p_psgp30->start();

    //std::cout << "Sleep for " << seconds << " seconds" << std::endl;
    sleep(seconds);

    p_psgp30->stop();
    //std::cout << "Stop SGP30 objects. Save debug data." << std::endl;

#ifdef SGP30_DEBUG
    p_psgp30->unload_debug_data(argc == 5 ? std::string(argv[4]) : std::string("/var/log/pi-robot/sgp30.csv"));
#endif
    //std::cout << "Release SGP30 objects " << std::endl;
    p_psgp30.reset();
    //std::cout << "Release Provider objects " << std::endl;
    p_psgp30.reset();

    //std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
