
/**
 * @file mcp23008.cpp
 * 
 * This library was built based on the Datasheet "MCP23008/MCP23S08 8-Bit I/O Expander with Serial Interface" from Microchip
 * 
 * @author Ricardo LIma Caratti (pu2clr@gmail.com)
 * @brief It is a Library to control the MCP23008 device.
 * @date 2021-01-06
 * 
 * This library can be freely distributed using the MIT Free Software model.
 * 
 * @copyright Copyright (c) 2020 Ricardo Lima Caratti
 */

#include "pu2clr_mcp23008.h"

#include <Wire.h>

/**
 * @brief Starts the MCP23008 
 * @details Starts the MCP23008 and sets the default values. 
 * @param i2c I2C address (0x20 ~ 0x27) - default 0x20
 * @param io  If GPIO_OUTPUT (0), all  GPIO PINS will configured to output
 *            If GPIO_INPUT  (255), all GPIO PINS will configured to input  
 *            You also can use a bitmask to configure some pins for input and other pins for output. 
 */
void MCP::setup(uint8_t i2c, uint8_t io) {
  
    Wire.begin(); //creates a Wire object
  
    this->i2cAddress = i2c;
    this->setRegister(REG_IODIR, io);    // All GPIO pins are configured to input (1)  or output (0)
    this->setGPIOS(0);                   // // Sets all port to 0
}  

/**
 * @brief Gets the corrent register information. 
 * @details Gets the current register content. 
 * @param reg  (0x00 ~ 0xA) see MCP23008 registers documentation 
 * @return uint8_t current register value
 */
uint8_t MCP::getRegister(uint8_t reg) {
    Wire.beginTransmission(this->i2cAddress);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((int) this->i2cAddress, (int) 1); 
    return Wire.read();
}

/**
 * @brief Sets a value to a given register
 * @details Sets a given 8 bit value to a given register.  
 * @param reg   (0x00 ~ 0xA) see MCP23008 registers documentation 
 * @param value value (8 bits)
 */
void MCP::setRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(this->i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission(); //ends communication with the device
}

/**
 * @brief Sets a value to the GPIO Register
 * @details A direct way to set a given value to deal with the GPIOs pins.
 * @param value (8 bits)
 */
void MCP::setGPIOS(uint8_t value) {
    this->gpios = value;
    setRegister(REG_GPIO, value);
    Wire.beginTransmission(i2cAddress);
}


/**
 * @brief Turns a given GPIO port on (high level)
 * @details Sets a given GPIO pin high
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::turnGpioOn(uint8_t gpio)
{
    uint8_t b = (1 << gpio);
    // Checks if it is already ON (avoid trafic on I2C)
    if ( (bool)(gpios & b) || gpio > 7 )
        return;
    gpios = gpios | b;
    setGPIOS(gpios);
}

/**
 * @brief Turns a given GPIO port off (low level)
 * @details Sets a given GPIO pin to low
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::turnGpioOff(uint8_t gpio)
{
    uint8_t b = (1 << gpio);
    // Checks if it is already OFF
    if ((gpios & b) == 0 || gpio > 7)
        return;
    gpios = gpios ^ b;
    setGPIOS(gpios);
}

/**
 * @brief Turns intenal pull up resistor ON  to  a given GPIO PIN (high level)
 * @details Activates the pull up resistor to a given GPIO pin
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::pullUpGpioOn(uint8_t gpio)
{
    uint8_t gppu; 

    if (gpio > 7) 
        return;

    gppu = getRegister(REG_GPPU); // Gets the current values of pull-up setup
    gppu |= 1 << gpio;
    setRegister(REG_GPPU, gppu); // Updates the values of pull-up setup
}

/**
 * @brief Turns intenal pull up resistor OFF  to a given GPIO PIN (low level)
 * @details Deactivates the pull up resistor to a given GPIO pin
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::pullUpGpioOff(uint8_t gpio)
{
    uint8_t gppu;
    if (gpio > 7)
        return;
    gppu = getRegister(REG_GPPU); // Gets the current values of pull-up setup
    gppu &= ~(1 << gpio);
    setRegister(REG_GPPU, gppu); // Updates the values of pull-up setup
}

/**
 * @brief Sets the IO Configurarion gerister
 * @details The IOCON register contains several bits for configuring the device:
 * @details TheSequentialOperation(SEQOP)controlsthe incrementing function of the address pointer. 
 * @details If the address pointer is disabled, the address pointer does not automatically increment after each byte is clocked during a serial transfer. 
 * @details This feature is useful when it is desired to continuously poll (read) or modify (write) a register.
 * @details TheSlewRate(DISSLW)bitcontrolstheslew rate function on the SDA pin. If enabled, the SDA slew rate will be controlled when driving from a high to a low.
 * @details TheHardwareAddressEnable(HAEN)controlbit enables/disables the hardware address pins (A1, A0) on the MCP23S08. This bit is not used on the MCP23008. The address pins are always enabled on the MCP23008.
 * @details TheOpen-Drain(ODR)controlbitenables/ disables the INT pin for open-drain configuration.
 * @details TheInterruptPolarity(INTPOL)controlbitsets the polarity of the INT pin. This bit is functional only when the ODR bit is cleared, configuring the INT pin as active push-pull.
 * 
 * @param INTPOL  This bit sets the polarity of the INT output pin. 1= Active-high. 0 = Active - low.
 * @param ODR     This bit configures the INT pin as an open-drain output. 1 = Open-drain. 0 = Active driver.
 * @param HAEN    Hardware Address Enable bit (MCP23S08 only). 1 = Enables.
 * @param DISSLW  Slew Rate control bit for SDA output. 1= Slewratedisabled. 0= Slewrateenabled.
 * @param SEQOP   Sequential Operation mode bit. 1 = Sequential operation disabled, address pointer does not increment.
 */
void MCP::setIoCon(uint8_t INTPOL, uint8_t ODR, uint8_t HAEN, uint8_t DISSLW, uint8_t SEQOP) {

    mcp23008_ioncon iocon;

    iocon.arg.INTPOL = INTPOL;
    iocon.arg.ODR = ODR;
    iocon.arg.HAEN = HAEN;
    iocon.arg.DISSLW = DISSLW;
    iocon.arg.SEQOP = SEQOP;

    this->setRegister(REG_IOCON, iocon.raw);
}
