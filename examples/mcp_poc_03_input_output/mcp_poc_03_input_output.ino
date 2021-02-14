/**
   This sketchs shows how to deal with input/output setup.
   Here you will see some ways to turn the gpio pins HIGH or LOW (output operations).
   Also you will see how to read values from the gpio pins (input operations).

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
   When the system starts, press any button. Each button starts a different sequence of LED activities

   Author: Ricardo Lima Caratti.
*/

#include <pu2clr_mcp23008.h>

MCP mcp;


void setup() {

  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
  while (!Serial); // Waiting for Serial Monitor


  mcp.setup(0x20, 0B00001111);  // GPIO 0 to 3 are input (buttons) and 4 to 7 are output (LEDs)
  mcp.setRegister(REG_GPPU, 0B00001111); // sets GPIO 0 to 3 with internal pull up resistors

  mcp.setGPIOS(0B11110000); // Turns All LEDS ON (Pins 4,5,6 and 7)
  delay(2000);
  turnLedsOff();
  // Turns pins/gpios 5 and 7 HIGH (On)
  mcp.gpioWrite(5, HIGH);
  mcp.gpioWrite(7, HIGH);
  delay(2000);
  // Turns pin/gpio 7 LOW (Off)
  mcp.gpioWrite(7, LOW);
  delay(2000);
  turnLedsOff();

}

void turnLedsOff() {
  delay(500);
  mcp.setGPIOS(0B00000000); // Turns All GPIO LOW (pins 0 to 7)
}

void loop() {

  // If any button is pressed the pin level will be set to LOW
  if ( mcp.gpioRead(0) == LOW ) {
    for (uint8_t i = 4; i < 8; i++)
    {
      mcp.turnGpioOn(i); // Turns the GPIO / port i on
      delay(300);
      mcp.turnGpioOff(i); // Turns the GPIO / port i off
      delay(300);
    }
    turnLedsOff();
  } else if ( mcp.gpioRead(1) == LOW ) {
    for (uint8_t i = 4; i < 8; i++)
    {
      mcp.turnGpioOn(i); // Turns the GPIO / port i on
      delay(300);
    }
    turnLedsOff();
  } else if ( mcp.gpioRead(2) == LOW ) {
    for (uint8_t i = 4; i < 7; i++)
    {
      mcp.turnGpioOn(i);    // Turns the GPIO / port i off
      mcp.turnGpioOn(i + 1);
      delay(250);
      mcp.turnGpioOff(i);    // Turns the GPIO / port i off
      mcp.turnGpioOff(i + 1);
      delay(250);
    }
    turnLedsOff();
  } else if ( mcp.gpioRead(3) == LOW ) {
    for (uint8_t i = 4; i < 8; i++)
    {
      mcp.setRegister(REG_GPIO, 0B11110000); // Turns All LEDS ON (pins 4,5,6 and 7)
      delay(300);
      mcp.setRegister(REG_GPIO, 0B00000000); // Turns All LEDS OFF (pins 0 to 7)
      delay(300);
    }
    turnLedsOff();
  }
}
