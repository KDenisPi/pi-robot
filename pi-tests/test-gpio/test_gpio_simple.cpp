#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "GpioProviderSimple.h"
#include "gpio.h"



using pprovider = std::shared_ptr<pirobot::gpio::GpioProviderSimple>;
using pgpio = std::shared_ptr<pirobot::gpio::Gpio>;

/*
* Simple GPIO test.
* 1. Try to initilize GPIO simple provider
* 2. Change GPIO configuration GPIO
* 3. Change GPIO level
*/


void print_info(const int gpios, const pirobot::gpio::GPIO_MODE* p_modes, pirobot::gpio::GPIO_MODE* g_modes, const int* g_levels)

void print_info(const int gpios, const pgpio* p_gpio, const pirobot::gpio::GPIO_MODE* p_modes, pirobot::gpio::GPIO_MODE* g_modes, const int* g_levels){
    for(int i = 0; i < gpios; i++){
        std::cout << i << " PIN: " << p_gpio[i]->>getPin() << " PMode: " << p_modes[i] << " GMode: " << g_modes[i] << "Level: " << g_levels << std::endl;
    }
}


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    int gpio_level = 0;
    const int num_gpios = 3;
    pgpio p_gpio[num_gpios];
    pirobot::gpio::GPIO_MODE p_modes[num_gpios];
    pirobot::gpio::GPIO_MODE g_modes[num_gpios];
    int g_levels[num_gpios];

    std::cout << "Started " << std::endl;

    std::cout << "Create provider" << std::endl;
    std::shared_ptr<pirobot::gpio::GpioProviderSimple> p_smp = std::make_shared<pirobot::gpio::GpioProviderSimple>();

    std::cout << "----- Modes --- " << std::endl << p_smp->print_mode() << std::endl;

    std::cout << "Create GPIO" << std::endl;
    p_gpio[0] = std::make_shared<pirobot::gpio::Gpio>(0, pirobot::gpio::GPIO_MODE::IN, p_smp);
    p_gpio[1] = std::make_shared<pirobot::gpio::Gpio>(1, pirobot::gpio::GPIO_MODE::IN, p_smp);
    p_gpio[2] = std::make_shared<pirobot::gpio::Gpio>(2, pirobot::gpio::GPIO_MODE::OUT, p_smp);


    for(int i = 0; i < num_gpios; i++){
        p_modes[i] = p_smp->getmode(p_gpio[i]->getPin());
        g_modes[i] = p_gpio[i]->getMode();
        g_levels[i] = p_gpio[i]->digitalRead();

        if(p_modes[i] != g_modes[i]){
           std::cout << "Mode on provider and GPIO levels are different after creating" << std::endl;
           success = false;
        }
    }

    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);
    sleep(1);

    std::cout << std::endl << "GPIO. Change mode " << std::endl;

    for(int i = 0; i < num_gpios; i++){
       //change mode  
       pirobot::gpio::GPIO_MODE new_mode = g_modes[i] == pirobot::gpio::GPIO_MODE::OUT ? pirobot::gpio::GPIO_MODE::IN : pirobot::gpio::GPIO_MODE::OUT;
       p_gpio[i]->setMode(new_mode);
       p_modes[i] = p_smp->getmode(p_gpio[i]->getPin());
       g_modes[i] = p_gpio[i]->getMode();

       if(p_modes[i] != new_mode || g_modes[i] != new_mode){
           std::cout << "Mode on provider and GPIO levels are different after changing" << std::endl;
           success = false;
       }
    }

    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);
    sleep(1);

    std::cout << std::endl << "GPIOs Get Level and Set Level ON" << std::endl;
    for(int i = 0; i < num_gpios; i++){
        p_gpio[i]->digitalWrite(1);
        g_levels[i] = p_gpio[i]->digitalRead();
        if(p_modes[i] == pirobot::gpio::GPIO_MODE::OUT && g_levels[i] != 1){
           std::cout << "Level for GPIO OUT was not set" << std::endl;
           success = false;
        }
    }
    
    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);
    sleep(1);

    std::cout << std::endl << "GPIOs Get Level and Set Level OFF" << std::endl;
    for(int i = 0; i < num_gpios; i++){
        p_gpio[i]->digitalWrite(0);
        g_levels[i] = p_gpio[i]->digitalRead();
        if(p_modes[i] == pirobot::gpio::GPIO_MODE::OUT && g_levels[i] != 0){
           std::cout << "Level for GPIO OUT was not set" << std::endl;
           success = false;
        }
    }

    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);

    std::cout << std::endl <<"Release GPIO objects " << std::endl;

    for(int i = 0; i < num_gpios; i++){
        p_gpio[i].reset();
    }

    std::cout << "Release Provider objects " << std::endl;
    p_smp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
