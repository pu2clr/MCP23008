
/**
 * @file mcp_poc_01.ino
 * @author Ricardo Lima Caratti (pu2clr@gmail.com)
 * @brief GPIO pins as output setup 
 * @details It is an example that shows how to use the MCP23008 GPIO pin as output
 * @copyright Copyright (c) 2021
 */

#include <pu2clr_mcp23008.h>

char sBuffer[80];

MCP mcp;

void setup() {

  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
  while (!Serial); // Waiting for Serial Monitor

  mcp.setup(0x20);
  mcp.turnGpioOn(MCP_GPIO1);
  mcp.turnGpioOn(MCP_GPIO5);

  showGpios();
  delay(2000);

  mcp.turnGpioOff(MCP_GPIO5);

  showGpios();

}

void showGpios() {
  Serial.println("\n*************\n");
  sprintf(sBuffer,"\nGPIOS: %d - %d\n", mcp.getGPIOS(), mcp.getRegister(REG_GPIO)) ;
  Serial.print(sBuffer);
  Serial.println(mcp.getGPIOS(),BIN);
  Serial.println(mcp.getRegister(REG_GPIO),BIN);
}

void loop() {

  mcp.turnGpioOn(MCP_GPIO1);
  mcp.turnGpioOn(MCP_GPIO5);
  mcp.turnGpioOn(MCP_GPIO7);
  showGpios();
  delay(2000);
  mcp.turnGpioOff(MCP_GPIO5);
  delay(1000);
  mcp.turnGpioOff(MCP_GPIO1);
  delay(1000);
  mcp.turnGpioOff(MCP_GPIO7);
  showGpios();
  delay(1000);
  mcp.turnGpioOn(MCP_GPIO1);
  delay(500);
  mcp.turnGpioOn(MCP_GPIO5);
  delay(500);
  mcp.turnGpioOn(MCP_GPIO7);
  showGpios();
  delay(1000);
  mcp.turnGpioOff(MCP_GPIO1);
  mcp.turnGpioOff(MCP_GPIO5);
  mcp.turnGpioOff(MCP_GPIO7);
  showGpios();
  delay(2000); 
}
