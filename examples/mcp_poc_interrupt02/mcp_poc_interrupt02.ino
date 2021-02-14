
/**
   This sketch shows how to deal with interrupts.
   This sketch uses the Serial Monitor 

   Arduino and MCP23008 setup

   | Device   | MCP23008 | Description |
   | -------- | -------- | ----------- |
   | Arduino  |          |             |
   |    A5    |  SCL (1) | I2C Clock   |
   |    A4    |  SDA (2) | I2C Data    |
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

   See schematic on https://github.com/pu2clr/MCP23008#gpio-as-input-and-output-setup

   Instructions:
   When the system starts, press any button and check the Serial Monitor.

   Author: Ricardo Lima Caratti.
 */

#include <pu2clr_mcp23008.h>

#define ARDUINO_INTERRUPT_PIN 2 

volatile bool mcp23008_event = false;

MCP mcp;

void setup() {

    uint8_t iocon;

    Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
    while (!Serial)
        ; // Waiting for Serial Monitor

    pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ARDUINO_INTERRUPT_PIN), checkMCP, CHANGE);

    mcp.setup(0x20, 255); // I2C address = 0x20 and all MCP23008 GPIO pins are configured as input
    
    iocon = mcp.getRegister(REG_IOCON); 
    iocon |= 0B00000110;
    mcp.setRegister(REG_IOCON,iocon);
    
    mcp.setRegister(REG_GPPU, 0B00001111); // Enables the pull up resistors on pins 0,1,2,3    
    mcp.setRegister(REG_GPINTEN,0B00001111); // The pins 0,1,2 and 3 of MCP23008 are now configured to lauch an interrupt
    mcp.setRegister(REG_DEFVAL,0B00001111);  // The pins 0,1,2 and 3 will be compared with 1. If it one of them was not equal to 1, than an interrupt will occur;
    mcp.setRegister(REG_INTCON,0); // Pin value is compared against the previous pin value.

    Serial.print("\n**** Make pins 0, 1, 2 or 3 low ****\n");  

}

/**
 * @brief Proccess the MCP23008 event
 * @details This function is called when some event in the MCP23008 GPIO input status is changed
 */
void checkMCP() {
    mcp23008_event = true;
}

int checkInput(uint8_t intcap) {

   for (int i = 0; i < 4; i++ ) 
      if ( !mcp.isBitValueHigh(intcap,i) ) return i;
      
   return -1;
}

void loop() {

    if ( mcp23008_event) { 
        uint8_t intcap = mcp.getRegister(REG_INTCAP);
        int button = checkInput(intcap);
        Serial.print("\nGPIO INPUT STATUS: ");
        Serial.print(intcap, BIN);
        if ( button >= 0 ) {
          Serial.print(" - GPIO: ");
          Serial.print(button);
        }
        mcp23008_event = false;
    }
}
