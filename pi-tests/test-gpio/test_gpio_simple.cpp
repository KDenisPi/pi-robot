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


void get_gpio_level(const pgpio pg);
void set_gpio_level(const pgpio pg, int val);
pirobot::gpio::GPIO_MODE get_gpio_mode(const pgpio pg);

void get_gpio_level(const pgpio pg) {
    std::cout << "GPIO " << pg->getPin() << " Get Level: " << pg->digitalRead() << std::endl;
}
void set_gpio_level(const pgpio pg, int val) {
    pg->digitalWrite(val);
    std::cout << "GPIO " << pg->getPin() << " Set Level: " << val << std::endl;
}

pirobot::gpio::GPIO_MODE get_gpio_mode(const pgpio pg){
    std::cout << "GPIO " << pg->getPin() << " Get Mode Provider: " << pg->get_provider()->getmode(pg->getPin()) << " Gpio: " << pg->getMode() << std::endl;
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
       get_gpio_mode(p_gpio_0);

       if(p_modes[i] != g_modes[i]){
           success = false;
       }
    }

    sleep(1);

    std::cout << std::endl << "GPIO. Change mode " << std::endl;

    for(int i = 0; i < num_gpios-1; i++){
       p_gpio[i]->setMode(pirobot::gpio::GPIO_MODE::OUT);
       p_modes[i] = p_smp->getmode(p_gpio[i]->getPin());
       g_modes[i] = p_gpio[i]->getMode();

       if(p_modes[i] != pirobot::gpio::GPIO_MODE::OUT || g_modes[i] != pirobot::gpio::GPIO_MODE::OUT){
           success = false;
       }
    }

    sleep(1);

    std::cout << std::endl << "GPIOs Get Level " << std::endl;
    for(int i = 0; i < num_gpios-1; i++){
       g_levels[i] = p_gpio[i]->digitalRead();
       get_gpio_level(p_gpio[i]);
    }

    std::cout << std::endl << "GPIOs Set Level ON" << std::endl;
    for(int i = 0; i < num_gpios-1; i++){
       g_levels[i] = p_gpio[i]->digitalRead();
       get_gpio_level(p_gpio[i]);             
                                                                                                                                                                                                                                 }
    set_gpio_level(p_gpio_0, 1);
    set_gpio_level(p_gpio_1, 2);
    set_gpio_level(p_gpio_2, 3);

    std::cout << std::endl << "GPIOs Get Level " << std::endl;
    get_gpio_level(p_gpio_0);
    get_gpio_level(p_gpio_1);
    get_gpio_level(p_gpio_2);

    sleep(3);

    std::cout << std::endl << "GPIOs Set Level OFF" << std::endl;
    set_gpio_level(p_gpio_0, 0);
    set_gpio_level(p_gpio_1, 0);
    set_gpio_level(p_gpio_2, 0);
                                                                                                                                                                                                                                                                                                                                 std::cout << std::endl << "GPIOs Get Level " << std::endl;
    get_gpio_level(p_gpio_0);
    get_gpio_level(p_gpio_1);
    get_gpio_level(p_gpio_2);

    sleep(3);

    std::cout << std::endl <<"Release GPIO objects " << std::endl;
    p_gpio_0.reset();
    p_gpio_1.reset();
    p_gpio_2.reset();

    std::cout << "Release Provider objects " << std::endl;
    p_smp.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
