#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "GpioProviderSimple.h"
#include "gpio.h"



using pprovider = std::shared_ptr<pirobot::gpio::GpioProviderSimple>;
using pgpio = std::shared_ptr<pirobot::gpio::Gpio>;

/*
* Pull UP Pull Down GPIO test.
* Preconfition: Concatenate two GPIOs. 
   GPIO 0 (PIN 11)- set to OUT, level 0
   GPIO 1 (PIN 12)- Set to IN, Pull mode Down
* 1. Initilize GPIO simple provider
* 2. Get value for GPIO 1 (should be 0)
* 3. Set Level UP (1) for GPIO 0
* 4. Get value for GPIO 1 (should be 1)
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
    int gpio_level = 0;
    const int num_gpios = 2;
    pgpio p_gpio[num_gpios];
    pirobot::gpio::GPIO_MODE p_modes[num_gpios];
    pirobot::gpio::GPIO_MODE g_modes[num_gpios];
    int g_levels[num_gpios];

    std::cout << "Started " << std::endl;

    std::cout << "Create provider" << std::endl;
    std::shared_ptr<pirobot::gpio::GpioProviderSimple> p_smp = std::make_shared<pirobot::gpio::GpioProviderSimple>();

    std::cout << "Create GPIO" << std::endl;
    p_gpio[0] = std::make_shared<pirobot::gpio::Gpio>(0, pirobot::gpio::GPIO_MODE::OUT, p_smp);
    p_gpio[1] = std::make_shared<pirobot::gpio::Gpio>(1, pirobot::gpio::GPIO_MODE::IN, p_smp);

    p_gpio[0]->pullUpDnControl(pirobot::gpio::PULL_MODE::PULL_OFF);
    p_gpio[1]->pullUpDnControl(pirobot::gpio::PULL_MODE::PULL_DOWN);

    for(int i = 0; i < num_gpios; i++){
        p_gpio[i]->digitalWrite(0); //Set to Low

        p_modes[i] = p_smp->getmode(p_gpio[i]->getPin());
        g_modes[i] = p_gpio[i]->getMode();
        g_levels[i] = p_gpio[i]->digitalRead();

        if(p_modes[i] != g_modes[i]){
           std::cout << "Mode on provider and GPIO levels are different after creating" << std::endl;
           success = false;
        }

        if(p_modes[i] == pirobot::gpio::GPIO_MODE::IN){
           if(g_levels[i] != 0){
              std::cout << "Low Level for GPIO IN detected incorrectly" << std::endl;
              success = false;
           }
           else
              std::cout << "Low Level for GPIO IN is correct (0)" << std::endl;
        }
    }

    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);
    sleep(1);

    std::cout << std::endl << "GPIO 0 Set Level ON. Check level on GPIO 1" << std::endl;
    p_gpio[0]->digitalWrite(1);

    for(int i = 0; i < num_gpios; i++){
        g_levels[i] = p_gpio[i]->digitalRead();

        if(p_modes[i] == pirobot::gpio::GPIO_MODE::IN){
           if(g_levels[i] != 1){
               std::cout << "High Level for GPIO IN detected incorrectly" << std::endl;
               success = false;
           }
           else
               std::cout << "High Level for GPIO IN is correct (1)" << std::endl;
        }
    }

    print_info(num_gpios, p_gpio, p_modes, g_modes, g_levels);
    p_gpio[0]->digitalWrite(0);

    std::cout << std::endl <<"Release GPIO objects " << std::endl;

    for(int i = 0; i < num_gpios; i++){
        p_gpio[i].reset();
    }

    std::cout << "Release Provider objects " << std::endl;
    p_smp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
