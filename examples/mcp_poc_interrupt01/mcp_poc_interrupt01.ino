
/**
   @file mcp_poc_02.ino
   @author Ricardo Lima Caratti (pu2clr@gmail.com)
   @brief GPIO pins as input and interrupt control
   @details This example show how to use MCP23008 GPIO pins as input and also how to configure the interrupt control
   @copyright Copyright (c) 2021
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
  mcp.pullUpGpioOn(GPIO1);          // Enables internal pullup resistor on gpio pin 1
  mcp.interruptGpioOn(GPIO1, HIGH); // Sets the GPIO pin 1 to deal with interrupt. The pin 1 will be compared with the value 1 (HIGH). It will be launch an interrupt if the pin 1 goes to level 0 (LOW).


  // GPIO pin 5 setup
  mcp.pullUpGpioOn(GPIO3);          // Enables internal pullup resistor on gpio pin 3
  mcp.interruptGpioOn(GPIO3, HIGH); // Sets the GPIO pin 3 to deal with interrupt. The pin 3 will be compared with the value 1 (HIGH). It will be launch an interrupt if the pin 1 goes to level 0 (LOW).

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

    if ( (intcap & 0B00000010) == 0 )   // checks if the button 1 (GPIO 1) is LOW
      button  = 1;
    else if ((intcap & 0B00001000) == 0) // checks if the button 3 (GPIO 3) is LOW
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
