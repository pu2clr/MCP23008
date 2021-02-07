/**
 * @file mcp23008.h
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

#include <Arduino.h>

// registers
#define REG_IODIR 0x00   //!< Controls the direction of the data I/O. When  a  bit  is  set,  the  corresponding  pin  becomes  an input.  When  a  bit  is  clear,  the  corresponding  pin becomes an output.
#define REG_IPOL 0x01    //!< The IPOL  register  allows  the  user  to  configure  the polarity on the corresponding GPIO port bits.
#define REG_GPINTEN 0x02 //!< The GPINTEN register controls the interrupt-on-change feature for each pin.
#define REG_DEFVAL 0x03  //!< The default  comparison  value  is  configured  in  the DEFVAL register.
#define REG_INTCON 0x04  //!< The INTCON register controls how the associated pin value is compared for the interrupt-on-change feature
#define REG_IOCON 0x05   //!< The IOCON register contains several bits for configuring the device. See method: setIoCon
#define REG_GPPU 0x06    //!< The GPPU register controls the pull-up resistors for the port  pins.
#define REG_INTF 0x07    //!< The INTF register reflects the interrupt condition on the port pins of any pin that is enabled for interrupts via the GPINTEN register.
#define REG_INTCAP 0x08  //!< The INTCAP register captures the GPIO port value at the  time  the  interrupt  occurred.
#define REG_GPIO 0x09    //!< The GPIO  register  reflects  the  value  on  the  port.
#define REG_OLAT 0x0A    //!< The OLAT  register  provides  access  to  the  output latches.

#define GPIO_INPUT 0xFF
#define GPIO_OUTPUT 0x00

/**
 * @brief IOCON register structure - I/O EXPANDER CONFIGURATION REGISTER (ADDR 0x05)
 * @details The IOCON register contains several bits for configuring the device. 
 */
typedef union
{
   struct
   {
      uint8_t DUMMY1 : 1; //!< Unimplemented
      uint8_t INTPOL : 1; //!< This bit sets the polarity of the INT output pin. 1= Active-high. 0 = Active - low.
      uint8_t ODR : 1;    //!< This bit configures the INT pin as an open-drain output. 1 = Open-drain. 0 = Active driver.
      uint8_t HAEN : 1;   //!< Hardware Address Enable bit (MCP23S08 only). 1 = Enables.
      uint8_t DISSLW : 1; //!< Slew Rate control bit for SDA output. 1= Slewratedisabled. 0= Slewrateenabled.
      uint8_t SEQOP : 1;  //!< Sequential Operation mode bit. 1 = Sequential operation disabled, address pointer does not increment.
      uint8_t DUMMY2 : 2; //!< Unimplemented
   } arg;
   uint8_t raw;
} mcp23008_ioncon;

class MCP
{

protected:
   uint8_t i2cAddress = 0x20; // Default i2c address
   uint8_t gpios = 0;         // REG_GPIO shadow register

public:
   void setup(uint8_t i2c = 0x20, uint8_t io = GPIO_OUTPUT);
   void setGPIOS(uint8_t value);
   uint8_t getRegister(uint8_t reg);
   void setRegister(uint8_t reg, uint8_t value);
   void turnGpioOn(uint8_t gpio);
   void turnGpioOff(uint8_t gpio);
   void pullUpGpioOn(uint8_t gpio);
   void pullUpGpioOff(uint8_t gpio);
   void setIoCon(uint8_t INTPOL, uint8_t ODR, uint8_t HAEN, uint8_t DISSLW, uint8_t SEQOP);
   void invertGpioPolarity();
   void interruptGpioOn(uint8_t gpio);

   /**
     * @ingroup group01
     * @brief Return the current MCP GPIO pin levels 
     * 
     * @return uint8_t 
     */
   inline uint8_t getGPIOS()
   {
      return this->gpios;
   };
};
