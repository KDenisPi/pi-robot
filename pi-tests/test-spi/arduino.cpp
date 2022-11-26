#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <chrono>
#include <bitset>


#include "logger.h"
#include "GpioProviderSimple.h"
#include "SPI.h"
#include "sledctrl_spi.h"


using pspi = std::shared_ptr<pirobot::spi::SPI>;
using pspi_cfg = pirobot::spi::SPI_config;
using provider = std::shared_ptr<pirobot::gpio::GpioProviderSimple>;
using gpio = std::shared_ptr<pirobot::gpio::Gpio>;
using sleds_spi = std::shared_ptr<pirobot::item::sledctrl::SLedCtrlSpi>;
using sled = std::shared_ptr<pirobot::item::SLed>;

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    logger::log_init("/var/log/pi-robot/arduino_log");
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Arduino");

    const uint32_t rgb = pirobot::item::SLed::rgb2uint32_t(0x20, 0x40, 0x80);
    std::cout << "RGB " << std::hex << rgb << std::endl;

    std::cout << "Prepare SPI configuration" << std::endl;
    pspi_cfg cfg;
    //cfg.speed[0] = pirobot::spi::SPI_SPEED::MHZ_25; // 25 Mhz

    std::cout << "Create GPIO provider" << std::endl;
    provider g_prov = std::make_shared<pirobot::gpio::GpioProviderSimple>();
    std::cout << g_prov->printConfig() << std::endl;

    gpio p_gpio_ce0 = std::make_shared<pirobot::gpio::Gpio>(19, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);
    gpio p_gpio_ce1 = std::make_shared<pirobot::gpio::Gpio>(18, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);

    gpio p_gpio_miso = std::make_shared<pirobot::gpio::Gpio>(20, pirobot::gpio::GPIO_MODE::IN, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);
    gpio p_gpio_mosi = std::make_shared<pirobot::gpio::Gpio>(21, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);
    gpio p_gpio_clk  = std::make_shared<pirobot::gpio::Gpio>(22, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);

    std::cout << p_gpio_ce0->to_string() << std::endl;
    std::cout << p_gpio_ce1->to_string() << std::endl;

    std::cout << "Create SPI provider" << std::endl;
    pspi p_pspi = std::make_shared<pirobot::spi::SPI>(std::string("PI_SPI"), cfg, p_gpio_ce0, p_gpio_ce1);
    std::cout << p_pspi->printConfig() << std::endl;

    {
        uint8_t spi_data[18] = {0xA1, 0xA2, 0xB1, 0xB2, 0xC1, 0xC2,0xA1, 0xA2, 0xB1, 0xB2, 0xC1, 0xC2,0xA1, 0xA2, 0xB1, 0xB2, 0xC1, 0xC2};
        uint8_t buff[18];

        std::cout << "p_gpio_ce0 " << p_gpio_ce0->is_High() << " p_gpio_ce1 " << p_gpio_ce1->is_High() << std::endl;

        std::cout << "Prepare to send data" << std::endl;

        if(p_pspi->set_channel_on(pirobot::spi::SPI_CHANNELS::SPI_0)){
            std::cout << "Sending data" << std::endl;
            std::cout << "p_gpio_ce0 " << p_gpio_ce0->is_High() << " p_gpio_ce1 " << p_gpio_ce1->is_High() << std::endl;

	    for(int i=0; i< sizeof(spi_data); i++){
                    memcpy(buff, spi_data, sizeof(spi_data));
	            int res = p_pspi->data_rw(buff, sizeof(buff));
        	    std::cout << i << " Result: " << res << " Data: " << std::bitset<8>(buff[0]) << std::endl;
            }
            p_pspi->set_channel_off(pirobot::spi::SPI_CHANNELS::SPI_0);
        }



/*        
        //Create Stripe LEDS object
        sled sled1 = std::make_shared<pirobot::item::SLed>(150, pirobot::item::SLedType::WS2812B, std::string("LEDS_P"));
        std::cout << sled1->printConfig() << std::endl;

        //Create SLED control object
        sleds_spi leds_ctrl = std::make_shared<pirobot::item::sledctrl::SLedCtrlSpi>(p_pspi, 1, std::string("PI_LEDS"));
        leds_ctrl->set_refresh_delay(std::chrono::microseconds(0));
        std::cout << leds_ctrl->printConfig() << std::endl;

        //Add Stripe LEDS
        if(leds_ctrl->add_sled(sled1)){
            const uint32_t rgb_1 = pirobot::item::SLed::rgb2uint32_t(0x20, 0x40, 0x80);
            leds_ctrl->add_copy_rgb(rgb_1, 0, sled1->leds());

            const uint32_t rgb_2 = pirobot::item::SLed::rgb2uint32_t(0x90, 0x30, 0x10);
            leds_ctrl->add_copy_rgb(rgb_2, 0, sled1->leds());

            if(leds_ctrl->initialize()){
                std::cout << "Transformation started. Waiting for 10 seconds" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }

            std::cout << "Stop SELD controller" << std::endl;
            leds_ctrl->stop();
        }
*/        
    }

    std::cout << "Release SPI Provider objects " << std::endl;
    p_pspi.reset();

    p_gpio_ce1.reset();
    p_gpio_ce0.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
