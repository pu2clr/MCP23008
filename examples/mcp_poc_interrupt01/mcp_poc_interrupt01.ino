
/**
 * @file mcp_poc_02.ino
 * @author Ricardo Lima Caratti (pu2clr@gmail.com)
 * @brief GPIO pins as input and interrupt control
 * @details This example show how to use MCP23008 GPIO pins as input and also how to configure the interrupt control 
 * @copyright Copyright (c) 2021
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
    mcp.setInterrupt(1,1); // Defines the behaviour of the interrupt
    
    // GPIO pin 1 setup
    mcp.pullUpGpioOn(1);   // Enables internal pullup resistor on gpio pin 1 
    mcp.interruptGpioOn(1,1); // Sets the GPIO pin 1 to deal with interrupt. The pin 1 will be compared with the value 1. It will be launch an interrupt if the pin 1 goes to level 0.


    // GPIO pin 5 setup
    mcp.pullUpGpioOn(5);   // Enables internal pullup resistor on gpio pin 5 
    mcp.interruptGpioOn(5,1); // Sets the GPIO pin 5 to deal with interrupt. The pin 5 will be compared with the value 1. It will be launch an interrupt if the pin 1 goes to level 0.

    
    Serial.print("\n**** Please, set the PCP23008 GPIO pin 1 or pin 6 high ****\n");  
    
    pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ARDUINO_INTERRUPT_PIN), checkMCP, CHANGE);  
}


/**
 * @brief Proccess the MCP23008 event
 * @details This function is called when some event in the MCP23008 GPIO input status is changed
 */
void checkMCP()
{
    mcp23008_event = true;
    intCount++;
}

void loop() {
    if (mcp23008_event)
    {
        uint8_t intcap =  mcp.getRegister(REG_INTCAP); // cleans the interrupt status 
        Serial.print("\nINTERRUPT: ");
        Serial.print(intCount);
        Serial.print(" - ");
        Serial.print(intcap, BIN);
        mcp23008_event = false;
    }
}
