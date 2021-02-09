
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

MCP mcp;

void setup() {

    uint8_t iocon;

    Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
    while (!Serial)
        ; // Waiting for Serial Monitor

    pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ARDUINO_INTERRUPT_PIN), checkMCP, CHANGE);

    mcp.setup(0x20, 255); // I2C address = 0x20 and all MCP23008 GPIO output pins are configured as input
    
    iocon = mcp.getRegister(REG_IOCON); 
    iocon |= 0B00000110;
    mcp.setRegister(REG_IOCON,iocon);
    
    mcp.setRegister(REG_GPPU, 0B01000001); // Enables the pins 0 and 6 of MCP23008 GPIO pull up resistors  
    mcp.setRegister(REG_GPINTEN,0B01000001); // the pins 0 and 6 of MCP23008 GPIO pins are configured to lauch an interrupt
    mcp.setRegister(REG_DEFVAL,0B01000001);  // The pins 0 and 6 will be compared with 1. If it one of them was not equal to 1, than an interrupt will occur;
    mcp.setRegister(REG_INTCON,0); // Pin value is compared against the previous pin value.
    // mcp.setRegister(REG_INTF,0);
    Serial.print("\n**** Make some PCP23008 GPIO pin high ****\n");  

}

/**
 * @brief Proccess the MCP23008 event
 * @details This function is called when some event in the MCP23008 GPIO input status is changed
 */
void checkMCP() {
    mcp23008_event = true;
    // noInterrupts();
}

void loop() {

    if ( mcp23008_event) { 
        uint8_t intcap = mcp.getRegister(REG_INTCAP);
        uint8_t intf = mcp.getRegister(REG_INTF);
        Serial.print("\nGPIO INPUT STATUS: ");
        Serial.print(intcap, BIN);
        Serial.print(" - ");
        Serial.print(intf, BIN);
        mcp23008_event = false;
        // interrupts();
    }
}
