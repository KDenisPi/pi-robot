/*
 * GpioProviderMCP230xx.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP230xx_H_
#define PI_LIBRARY_GPIOPROVIDERMCP230xx_H_

#include <memory>

#include "I2C.h"
#include "GpioProvider.h"

namespace pirobot {
namespace gpio {

/*
• If BANK = 1, the registers associated with each port are segregated. Registers associated with 
PORTA are mapped from address 00h - 0Ah and registers associated with PORTB are mapped  from 10h-1Ah.
• If BANK = 0, the A/B registers are paired. For example, IODIRA is mapped to address 00h and 
IODIRB is mapped to the next address (address 01h). The mapping for all registers is from 00h-15h

The MIRROR bit controls how the INTA and INTB pins function with respect to each other.
• When MIRROR = 1, the INTn pins are functionally OR’ed so that an interrupt on either port will cause  both pins to activate.
• When MIRROR = 0, the INT pins are separated. Interrupt conditions on a port will cause its respective INT pin to activate.

The Sequential Operation (SEQOP) controls the incrementing function of the Address Pointer. If the
address pointer is disabled, the Address Pointer does not automatically increment after each byte is clocked
during a serial transfer. This feature is useful when it is desired to continuously poll (read) or modify (write) a register.

The Slew Rate (DISSLW) bit controls the slew rate function on the SDA pin. If enabled, the SDA slew rate
will be controlled when driving from a high to low.

The Hardware Address Enable (HAEN) bit enables/disables hardware addressing on the
MCP23S17 only. The address pins (A2, A1 and A0) must be externally biased, regardless of the HAEN bit value.
If enabled (HAEN = 1), the device’s hardware address matches the address pins.
If disabled (HAEN = 0), the device’s hardware address is A2 = A1 = A0 = 0

The Open-Drain (ODR) control bit enables/disables the INT pin for open-drain configuration. Setting this bit overrides the INTPOL bit.

The Interrupt Polarity (INTPOL) sets the polarity of the INT pin. This bit is functional only when the ODR bit is
cleared, configuring the INT pin as active push-pull.

bit 7 BANK: Controls how the registers are addressed
1 = The registers associated with each port are separated into different banks.
0 = The registers are in the same bank (addresses are sequential).

bit 6 MIRROR: INT Pins Mirror bit
1 = The INT pins are internally connected
0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB

bit 5 SEQOP: Sequential Operation mode bit
1 = Sequential operation disabled, address pointer does not increment.
0 = Sequential operation enabled, address pointer increments.

bit 4 DISSLW: Slew Rate control bit for SDA output
1 = Slew rate disabled
0 = Slew rate enabled

bit 3 HAEN: Hardware Address Enable bit (MCP23S17 only) (Note 1)
1 = Enables the MCP23S17 address pins.
0 = Disables the MCP23S17 address pins.

bit 2 ODR: Configures the INT pin as an open-drain output
1 = Open-drain output (overrides the INTPOL bit.)
0 = Active driver output (INTPOL bit sets the polarity.)

bit 1 INTPOL: This bit sets the polarity of the INT output pin
1 = Active-high
0 = Active-low

bit 0 Unimplemented: Read as ‘0’
*/

enum IOCON : uint8_t {
    UNIMPLEMENTED = 0x01,
    INTPOL = 0x02,
    ODR = 0x04,
    HAEN = 0x08,
    DISSLW = 0x10,
    SEQOP = 0x20,
    MIRROR = 0x40,
    BANK = 0x80
};

/*
The 16-bit I/O port functionally consists of two 8-bit
ports (PORTA and PORTB). The MCP23X17 can be
configured to operate in the 8-bit or 16-bit modes via
IOCON.BANK
*/

enum MCP23X17_B0 : uint8_t {
    IODIRA      = 0x00,
    IODIRB      = 0x01,
    IPOLA       = 0x02,
    IPOLB       = 0x03,
    GPINTENA    = 0x04,
    GPINTENB    = 0x05,
    DEFVALA     = 0x06,
    DEFVALB     = 0x07,
    INTCONA     = 0x08,
    INTCONB     = 0x09,
    IOCONA      = 0x0A,
    IOCONB      = 0x0B,
    GPPUA       = 0x0C,
    GPPUB       = 0x0D,
    INTFA       = 0x0E,
    INTFB       = 0x0F,
    INTCAPA     = 0x10,
    INTCAPB     = 0x11,
    GPIOA       = 0x12,
    GPIOB       = 0x13,
    OLATA       = 0x14,
    OLATB       = 0x15
};

enum MCP23X17_B1 : uint8_t {
    IODIRA_B1      = 0x00,
    IODIRB_B1      = 0x10,
    IPOLA_B1       = 0x01,
    IPOLB_B1       = 0x11,
    GPINTENA_B1    = 0x02,
    GPINTENB_B1    = 0x12,
    DEFVALA_B1     = 0x03,
    DEFVALB_B1     = 0x13,
    INTCONA_B1     = 0x04,
    INTCONB_B1     = 0x14,
    IOCONA_B1      = 0x05,
    IOCONB_B1      = 0x15,
    GPPUA_B1       = 0x06,
    GPPUB_B1       = 0x16,
    INTFA_B1       = 0x07,
    INTFB_B1       = 0x17,
    INTCAPA_B1     = 0x08,
    INTCAPB_B1     = 0x18,
    GPIOA_B1       = 0x09,
    GPIOB_B1       = 0x19,
    OLATA_B1       = 0x0A,
    OLATB_B1       = 0x1A
};


class GpioProviderMCP230xx: public GpioProvider{
public:
    GpioProviderMCP230xx(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr, const int pins);
    virtual ~GpioProviderMCP230xx();

    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void setmode(const int pin, const gpio::GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) override;

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

    // Return current state of GPIOs
    uint8_t dgtRead8(const int pin = 0){
       return get_OLAT(pin);
    }

    //Write 8-bit value
    void dgtWrite8(const uint8_t value, const int pin = 0){
        int gpio = get_GPIO_addr(pin);
        _i2c->I2CWriteReg8(m_fd, gpio, value);
        set_OLAT(value, pin);
    }

private:
    //Get address for GPIO register
    virtual const uint8_t get_GPIO_addr(const int pin) = 0;
    //Get address IODIR (IO direction) register
    virtual const uint8_t get_IODIR_addr(const int pin) = 0;
    //Get address GPPU (Pull-Up) register
    virtual const uint8_t get_GPPU_addr(const int pin) = 0;

    //Get current value for register data
    virtual unsigned int get_OLAT(const int pin) = 0;
    //Save current value register data
    virtual void set_OLAT(const unsigned int value, const int pin) = 0;

protected:
    uint8_t _i2caddr;
    int m_fd;
    std::shared_ptr<pirobot::i2c::I2C> _i2c;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP230xx_H_ */
