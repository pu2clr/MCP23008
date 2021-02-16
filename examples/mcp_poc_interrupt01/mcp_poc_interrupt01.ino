/**
   This sketch shows how to deal with interrupts. 
   
   See schematic on https://github.com/pu2clr/MCP23008#internal-interrupt-setup

   Arduino and MCP23008 setup

   | Device   | MCP23008 | Description |
   | -------- | -------- | ----------- |
   | Arduino  |          |             |
   |    A5    |  SCL (1) | I2C Clock   |
   |    A4    |  SDA (2) | I2C Data    |
   |    D2    |  INT     | Interrupt   |
   |   RST    |  RESET   | *1          | 
   | Buttons  |          |             |
   |   SW0    |  GPIO 0  |             |
   |   SW1    |  GPIO 1  |             |
   |   SW2    |  GPIO 2  |             |
   |   SW3    |  GPIO 3  |             |
   |  LEDs    |          |             |
   |  LED 0   |  GPIO 4  |             |
   |  LED 1   |  GPIO 5  |             |
   |  LED 2   |  GPIO 6  |             |
   |  LED 3   |  GPIO 7  |             |
   |   VCC    |  RESET   |             |

   Instructions:
   When the system starts, press any button and check the Serial Monitor.

   Author: Ricardo Lima Caratti.
*/

#include <pu2clr_mcp23008.h>

#define ARDUINO_INTERRUPT_PIN 2

volatile bool mcp23008_event = false;
volatile uint16_t intCount = 0;

MCP mcp;

void setup() {

  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
  while (!Serial);


  mcp.setup();
  mcp.setInterrupt(INTERRUPT_INTPOL_ACTIVE_HIGH, INTERRUPT_ODR_OPEN_DRAIN); // Defines the behaviour of the interrupt

  // GPIO pin 1 setup
  mcp.pullUpGpioOn(MCP_GPIO1);          // Enables internal pullup resistor on gpio pin 1
  mcp.interruptGpioOn(MCP_GPIO1, HIGH); // Sets the GPIO pin 1 to deal with interrupt. The pin 1 will be compared with the value 1 (HIGH). It will be launch an interrupt if the pin 1 goes to level 0 (LOW).


  // GPIO pin 5 setup
  mcp.pullUpGpioOn(MCP_GPIO3);          // Enables internal pullup resistor on gpio pin 3
  mcp.interruptGpioOn(MCP_GPIO3, HIGH); // Sets the GPIO pin 3 to deal with interrupt. The pin 3 will be compared with the value 1 (HIGH). It will be launch an interrupt if the pin 1 goes to level 0 (LOW).

  Serial.print("\n**** Please, press the buttons 1 or 3  ****\n");

  pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ARDUINO_INTERRUPT_PIN), checkMCP, CHANGE);
}


/**
   @brief Proccess the MCP23008 event
   @details This function is called when some event in the MCP23008 GPIO input status is changed
*/
void checkMCP()
{
  mcp23008_event = true;
  intCount++;
}

void loop() {
  if (mcp23008_event)
  {
    uint8_t intcap =  mcp.getRegister(REG_INTCAP); // gets the button pressed (GPIO) and cleans the interrupt status
    int button;

    if ( !mcp.isBitValueHigh(intcap,1) )   // checks if the button 1 (register INTCAP 1) is LOW
      button  = 1;
    else if (!mcp.isBitValueHigh(intcap,3)) // checks if the button 3 (register INTCAP 3) is LOW
      button = 3;
    else
      button = -1;

    if ( button >= 0 ) {
      Serial.print("\nINTERRUPT: ");
      Serial.print(intCount);
      Serial.print(" - ");
      Serial.print(intcap, BIN);
      Serial.print(" - You have pressed the button: ");
      Serial.print(button);
    }
    mcp23008_event = false;
  }
}
