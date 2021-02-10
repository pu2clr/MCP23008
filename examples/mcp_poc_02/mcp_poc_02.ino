#include <pu2clr_mcp23008.h>

MCP mcp;

void setup() {
    mcp.setup(0x20,0);   // I2C address 0x20; All GPIO port/pin are configured as output
}

void loop() {
    for (uint8_t i = 0; i < 8; i++)
    {
        mcp.turnGpioOn(i); // Turns the GPIO / port i on
        delay(1000);
        mcp.turnGpioOff(i); // Turns the GPIO / port i off
    }
    delay(2000);
    for (uint8_t i = 0; i < 8; i++)
    {
        mcp.turnGpioOn(i); // Turns the GPIO / port i on
        delay(500);
    }
    delay(2000);
    for (uint8_t i = 0; i < 8; i++)
    {
        mcp.turnGpioOff(i); // Turns the GPIO / port i off
        delay(500);
    }
    delay(5000);
}
