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

#define INTERRUPT_ODR_OPEN_DRAIN 1     //!< Open-drain output (overrides the INTPOL bit).
#define INTERRUPT_ODR_ACTIVE_DRIVE 0   //!< Active driver output (INTPOL bit sets the polarity).
#define INTERRUPT_INTPOL_ACTIVE_HIGH 1 //!< Active high - polarity of the INT output pin
#define INTERRUPT_INTPOL_ACTIVE_LOW 0  //!< Active low  - polarity of the INT output pin

#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO6 6
#define GPIO7 7

#define GET_BIT_CONTENT(x,y) ( x & (1 << y) )  // 0 or != 0


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
   uint8_t i2cAddress = 0x20; //!< Default i2c address
   uint8_t gpios = 0;         //!< REG_GPIO shadow register
   uint8_t intcap = 0;
   uint8_t intf = 0;
   int reset_pin =  -1;       //!< Digital Arduino pin to control the MCP2300 RESET

public:
   uint8_t lookForDevice(); 
   void reset();
   void setup(uint8_t i2c = 0x20, uint8_t io = GPIO_OUTPUT, int reset_pint = -1);
   uint8_t getRegister(uint8_t reg);
   void setRegister(uint8_t reg, uint8_t value);
   void turnGpioOn(uint8_t gpio);
   void turnGpioOff(uint8_t gpio);
   void pullUpGpioOn(uint8_t gpio);
   void pullUpGpioOff(uint8_t gpio);
   void setIoCon(uint8_t INTPOL, uint8_t ODR, uint8_t HAEN, uint8_t DISSLW, uint8_t SEQOP);
   void invertGpioPolarity();
   void setInterrupt(uint8_t polatity = 0, uint8_t openDrainOutput = 0);
   void interruptGpioOn(uint8_t gpio, uint8_t bitCompare = 1);
   bool gpioRead(uint8_t gpio);
   void gpioWrite(uint8_t gpio, uint8_t value);

   /**
   * @ingroup group01
   * @brief Returns the current MCP GPIO pin levels 
   * 
   * @return uint8_t 
   */
   inline uint8_t getGPIOS()
   {
      this->gpios = getRegister(REG_GPIO);
      return this->gpios;
   };

   /**
   * @ingroup group01
   * @brief Sets a value to the GPIO Register
   * @details A direct way to set a given value to deal with the GPIOs pins.
   * @param value (8 bits)
   */
   inline void setGPIOS(uint8_t value) {
       this->setRegister(REG_GPIO, value);
   }

   /**
     * @ingroup group01
     * @brief Returns the last value of INTCAP register (value immediately after the last interrupt event)
     * 
     * @return uint8_t 
     */
   inline uint8_t getINTCAP()
   {
      this->intcap = getRegister(REG_INTCAP);
      return this->intcap;
   };

   /**
     * @ingroup group01
     * @brief Returns the last value of INTCAP register (value immediately after the last interrupt event)
     * @details The INTF register reflects the interrupt condition on the port pins of any pin that is enabled for interrupts via the GPINTEN register. A ‘set’ bit indicates that the associated pin caused the interrupt.
     * @return uint8_t 
     */
   inline uint8_t getINTF()
   {
      this->intf = getRegister(REG_INTF);
      return this->intf;
   };

   /**
    * @ingroup group01
    * @brief Checks if the Bit Value of a given bit position is high
    * @details This funcion is useful to extract a bit value (0 or 1) from a given MCP23008 register
    * @param byteValue byte or register value
    * @param bitNumber bit offset (number / possition from 0 to 7)
    * @return true if the bit position is 1 (high)
    */
   inline bool isBitValueHigh(uint8_t byteValue, uint8_t bitNumber) {
      return (byteValue & (1 << bitNumber)) != 0;
   }
};
