#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "GpioProviderSimple.h"
#include "gpio.h"



using pprovider = std::shared_ptr<pirobot::gpio::GpioProviderSimple>;
using pgpio = std::shared_ptr<pirobot::gpio::Gpio>;

/*
* Pull UP Pull Down with Edge/Level support GPIO test.
* Preconfition: Concatenate two GPIOs. 
   GPIO 0 (PIN 11)- set to OUT, level 0
   GPIO 1 (PIN 12)- Set to IN, Pull mode Down
* 1. Initilize GPIO simple provider
* 2. Set Level UP (1) and Down for GPIO 0
* 3. Callback function should detect changed value for GPIO 1
*/


void print_info(const int gpios, const pirobot::gpio::GPIO_MODE* p_modes, pirobot::gpio::GPIO_MODE* g_modes, const int* g_levels);

void print_info(const int gpios, const pgpio* p_gpio, const pirobot::gpio::GPIO_MODE* p_modes, pirobot::gpio::GPIO_MODE* g_modes, const int* g_levels){
    for(int i = 0; i < gpios; i++){
        std::cout << i << " PIN: " << p_gpio[i]->getPin() << " PMode: " << p_modes[i] << " GMode: " << g_modes[i] << " Level: " << g_levels[i] << std::endl;
    }
}


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    int num_gpios = 2;
    pgpio p_gpio[2];

    std::cout << "Started " << std::endl;

    std::cout << "Create provider" << std::endl;
    std::shared_ptr<pirobot::gpio::GpioProviderSimple> p_smp = std::make_shared<pirobot::gpio::GpioProviderSimple>();

    std::cout << "Create GPIO" << std::endl;
    p_gpio[0] = std::make_shared<pirobot::gpio::Gpio>(0, pirobot::gpio::GPIO_MODE::OUT, p_smp);
    p_gpio[1] = std::make_shared<pirobot::gpio::Gpio>(1, pirobot::gpio::GPIO_MODE::IN, p_smp);

    std::cout << "GPIO Pull Mode" << std::endl;
    p_gpio[0]->pullUpDnControl(pirobot::gpio::PULL_MODE::PULL_OFF);
    p_gpio[1]->pullUpDnControl(pirobot::gpio::PULL_MODE::PULL_DOWN);

    std::cout << std::endl <<" Change GPIO OUT 0 (17) to High (1)" << std::endl;
    p_gpio[0]->Low(); //High();

    std::cout << "GPIO Edge" << std::endl;
    //p_gpio[0]->set_egde_level(pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_RAISING);
    p_gpio[1]->set_egde_level(pirobot::gpio::GPIO_EDGE_LEVEL::LEVEL_HIGH); //EDGE_RAISING);
    sleep(1);

    std::cout << std::endl <<" Start gpio edge" << std::endl;
    p_smp->start();
    //p_gpio[0]->High();
    //sleep(2);

    //std::cout << std::endl <<" Change GPIO OUT 0 (17) to High (0) and wait" << std::endl;
//    p_gpio[0]->Low();
//    sleep(3);

    std::cout << std::endl <<"Release GPIO objects " << std::endl;

    for(int i = 0; i < num_gpios; i++){
        p_gpio[i].reset();
    }

    std::cout << "Release Provider objects " << std::endl;
    p_smp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
