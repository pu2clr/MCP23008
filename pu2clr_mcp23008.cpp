
/**
 * @mainpage MCP23008 Arduino Library implementation 
 * @details This library was built based on the Datasheet "MCP23008 8-Bit I/O Expander with Serial Interface" from Microchip
 * @details The MCP23008 device provides 8-bit, general purpose, parallel I/O expansion. It can be controlled via I²C bus applications. It is a great and inexpensive device that allow you to add more peripherals to be controlled by your Arduino board via I²C protocol.
 * @details Some Arduino projects may require more pins than the Arduino actually has. In these cases, you can use up to 8 MCP23008 devices using only the I²C bus (two Arduino pins) and add up to 64 input / output ports to your project. 
 * @details This library can be freely distributed using the MIT Free Software model.
 * @details This library uses the I²C communication protocol and implements most important functions offered by MCP23008 device from MicroChip. It also has primitive functions that make it easier to implement commands that may not have been implemented yet. The main features implemented can be seen below:
 * @details 1) GPIO individual control (8 I/O pins)
 * @details 2) Reading and writing Access to all registers (0x00 ~ 0xA) 
 * @details 3) Internal pull up GPIO resistors control
 * @details 4) I²C address customization (0x20 ~ 0x27)
 * @details 5) Internal Interrupt feature setup 
 * @details 6) Reset control
 * @author Ricardo LIma Caratti (pu2clr@gmail.com)
 * @brief It is a Library to control the MCP23008 device.
 * @date 2021-01-06
 * @copyright Copyright (c) 2021 Ricardo Lima Caratti
 */

#include "pu2clr_mcp23008.h"

#include <Wire.h>

/** @defgroup group01 MCP23008 functions */

/**
 * @ingroup group01
 * @brief Look for MCP23008 device I2C Address
 * @details This method will look for a valid MCP23008 device adress between 0x20 and 0x27 
 * @return uint8_t the I2C address of the first MCP23008 device connect in the I2C bus
 */
uint8_t MCP::lookForDevice() {
    int err = 0;
    Wire.begin();
    for (int addr = 0x20; addr <= 0x27; addr++)
    {
        Wire.beginTransmission(addr);
        err = Wire.endTransmission();
        if (err == 0)
            return addr;
    }
    // Any MCP23008 device was found
    return 0;
}

/**
 * @ingroup group01
 * @brief Resets the MCP23008
 * @details this method can be used if you want to control the reset of the MCP23008 device. You need to select the Arduino pin via setup method.
 * @details You can used the MCP23008 RESET pin connected to the Arduino RESET pin. In this case, you not need to use this function.
 * @see setup()
 */
void MCP::reset() {
    if (this->reset_pin >= 0  ) {
        pinMode(this->reset_pin, OUTPUT);
        delay(5);
        digitalWrite(this->reset_pin, LOW);
        delay(5);
        digitalWrite(this->reset_pin, HIGH);
        delay(5);
    }
}

/**
 * @ingroup group01
 * @brief Starts the MCP23008 
 * @details Starts the MCP23008 and sets the default values. 
 * @param i2c I2C address (0x20 ~ 0x27) - default 0x20
 * @param io  If GPIO_OUTPUT (0), all  GPIO PINS will configured to output
 *            If GPIO_INPUT  (255), all GPIO PINS will configured to input  
 *            You also can use a bitmask to configure some pins for input and other pins for output.
 * @param reset_pin if you want to control the reset, select an Arduino pin to do that.  
 */
void MCP::setup(uint8_t i2c, uint8_t io, int reset_pin ) {
  
    Wire.begin(); //creates a Wire object
  
    this->i2cAddress = i2c;
    this->setRegister(REG_IODIR, io);    // All GPIO pins are configured to input (1)  or output (0)
    this->setGPIOS(0);                   // // Sets all port to 0

    this->reset_pin = reset_pin;
    this->reset();
}

/**
 * @ingroup group01
 * @brief Gets the corrent register information. 
 * @details Gets the current register content. 
 * @param reg  (0x00 ~ 0xA) see MCP23008 registers documentation 
 * @return uint8_t current register value
 */
uint8_t MCP::getRegister(uint8_t reg) {
    delayMicroseconds(2000);
    Wire.beginTransmission(this->i2cAddress);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((int) this->i2cAddress, (int) 1); 
    return Wire.read();
}

/**
 * @ingroup group01
 * @brief Sets a value to a given register
 * @details Sets a given 8 bit value to a given register.  
 * @param reg   (0x00 ~ 0xA) see MCP23008 registers documentation 
 * @param value value (8 bits)
 */
void MCP::setRegister(uint8_t reg, uint8_t value) {
    delayMicroseconds(2000);
    Wire.beginTransmission(this->i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission(); //ends communication with the device
}


/**
 * @ingroup group01
 * @brief Turns a given GPIO port on (high level)
 * @details Sets a given GPIO pin high
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::turnGpioOn(uint8_t gpio)
{
    // Checks if it is already ON (avoid trafic on I2C)
    if ( gpio > 7 )
        return;

    uint8_t b = (1 << gpio);

    gpios = this->getGPIOS() | b;
    this->setGPIOS(gpios);
}

/**
 * @ingroup group01
 * @brief Turns a given GPIO port off (low level)
 * @details Sets a given GPIO pin to low
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::turnGpioOff(uint8_t gpio)
{
    // Checks if it is already OFF
    if (gpio > 7)
        return;
    uint8_t b = (1 << gpio);    
    gpios = this->getGPIOS() & ~b;
    this->setGPIOS(gpios);
}

/**
 * @ingroup group01
 * @brief Reads the status (high or low) or a given GPIO
 * @details Returns true if the gpio is hight or fale if it is low.
 * @param gpio pin number
 */
bool MCP::gpioRead(uint8_t gpio) {
    // Checks if it is already OFF
    if (gpio > 7) return false;
    return getRegister(REG_GPIO) & (1 << gpio);
}

/**
 * @ingroup group01
 * @brief Sets a given value (high(1) or low(0) ) to a given gpio pin
 * @details It is like the turnGpioOn()
 * @param gpio pin number
 * @param value 1 = High;  0 =  Low 
 */
void MCP::gpioWrite(uint8_t gpio, uint8_t value) {
    if (gpio > 7)
        return;
    uint8_t currentGpio = this->getRegister(REG_GPIO) ;
    this->setRegister(REG_GPIO, (currentGpio & ~(1 << gpio)) | (value << gpio) );
}



/**
 * @ingroup group01
 * @brief Turns intenal pull up resistor ON  to  a given GPIO PIN (high level)
 * @details Activates the pull up resistor to a given GPIO pin
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::pullUpGpioOn(uint8_t gpio)
{
    uint8_t gppu; 

    if (gpio > 7) 
        return;

    gppu = this->getRegister(REG_GPPU); // Gets the current values of pull-up setup
    gppu |= 1 << gpio;
    this->setRegister(REG_GPPU, gppu); // Updates the values of pull-up setup
}

/**
 * @ingroup group01
 * @brief Turns intenal pull up resistor OFF  to a given GPIO PIN (low level)
 * @details Deactivates the pull up resistor to a given GPIO pin
 * @param gpio the GPIO/PIN number (0-7)
 */
void MCP::pullUpGpioOff(uint8_t gpio)
{
    uint8_t gppu;
    if (gpio > 7)
        return;
    gppu = this->getRegister(REG_GPPU); // Gets the current values of pull-up setup
    gppu &= ~(1 << gpio);
    this->setRegister(REG_GPPU, gppu); // Updates the values of pull-up setup
}

/**
 * @ingroup group01
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

/**
 * @brief Returns the IOCON content 
 * @details This function returns a mcp23008_ioncon datatype with the IOCON register content 
 * @see mcp23008_ioncon
 * @return mcp23008_ioncon content
 */
mcp23008_ioncon MCP::getIoCon()
{
    uint8_t iocon = this->getRegister(REG_IOCON);
    mcp23008_ioncon aux;
    aux.raw = iocon;
    return aux;
}

/**
 * @ingroup group01
 * @brief Inverts the polarity of the __all__ GPIO port bits.
 * @details The IPOL register allows the user to configure the polarity on the corresponding GPIO port bits.
 * @details If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin.
 * @details Use the primitive setRegister(REG_IPOL, reg_value) if you ant to invert only a specific gpio.
 * @see setRegister
 */
void MCP::invertGpioPolarity() {
    uint8_t iopol; 
    iopol = this->getRegister(REG_IPOL); // Gets the current setup
    this->setRegister(REG_IPOL, ~iopol);
}

/**
 * @ingroup group01
 * @brief Configures the MCP23008 interrupt feature.
 * @details The INT output pin will be activated when an internal interrupt occurs. 
 * @details The interrupt block can be configured by the following registers: GPINTEN, DEFVAL, INTCON and IOCON(ODRandINPOL).
 * @details This method work on IOCON. 
 * 
 * @param polatity  sets the polarity of the INT output pin. 1 = Active-high; 0 = Active-low.
 * @param openDrainOutput configures the INT pin as an open-drain output. 1 = Open-drain output (overrides the INTPOL bit). 0 = Active driver output (INTPOL bit sets the polarity).
 */
void MCP::setInterrupt(uint8_t polatity , uint8_t openDrainOutput) {
    uint8_t intpol = polatity << 1;
    uint8_t odr = openDrainOutput << 2;
    uint8_t iocon = this->getRegister(REG_IOCON); // Gets the current value of the REG_IOCON register
    iocon |= (intpol | odr);
    this->setRegister(REG_IOCON, iocon); // Save the new REG_IOCON registervalue
}

/**
 * @ingroup group01
 * @brief Sets the interrupt-on-change feature to a given GPIO pin 
 * @details The GPINTEN register controls the interrupt-on-change feature for each pin.
 * @details If a bit is set, the corresponding pin is enabled for interrupt-on-change. 
 * @details The DEFVAL and INTCON registers must also be configured if any pins are enabled for interrupt-on-change.
 * @details if you want to configure more than one GPIO at once, use the primitive  setRegister(REG_GPINTEN, reg);
 * @details If enabled (via GPINTEN and INTCON) to compare against the DEFVAL register, an opposite value on the associated pin will cause an interrupt to occur.
 * 
 * @param gpio GPIO / PIN you want to configure
 * @param bitCompare The default comparison bit value that should be configured in the DEFVAL register.  
 */
void MCP::interruptGpioOn(uint8_t gpio, uint8_t bitCompare) {
    uint8_t reg;
    uint8_t bitX; 

    if (gpio > 7)
        return;

    bitX = 1 << gpio;


    // Sets the gpio pin as input
    reg = this->getRegister(REG_IODIR); // Gets the current values of REG_IODIR
    reg |= bitX;
    this->setRegister(REG_IODIR, reg); // Updates the values of the  REG_IODIR register

    // Enables the GPIO pin to deal with interrupt  
    reg = this->getRegister(REG_GPINTEN); // Gets the current values of GPINTEN
    reg |= bitX;
    this->setRegister(REG_GPINTEN, reg); // Updates the values of the  GPINTEN register

    // Defines the criteria to launch the interrupt
    reg = this->getRegister(REG_DEFVAL); // Gets the current values of REG_DEFVAL
    reg = (reg & (~bitX) ) | (bitCompare << gpio); // Turns high or low the given GPIO port/pin based on bitCompare
    this->setRegister(REG_DEFVAL, reg);

}
