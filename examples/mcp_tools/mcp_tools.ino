#include <pu2clr_mcp23008.h>
#include <Wire.h>


char buffer[80];
MCP mcp;

void setup(){

    uint8_t i2cAddr;

    Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
    while (!Serial);    // Waiting for Serial Monitor

    
    // Try to get the I2C bus address automatically - You can use the MCP23008 A0, A1 and A3 pins to customize the I2C bus address
    i2cAddr = getI2cAddress();
    
     if (i2cAddr > 0 ) {
        // Starts the MCP23008 device;  all GPIO pins will be output
        mcp.setup(i2cAddr, GPIO_OUTPUT);
        sprintf(buffer, "\nThe device was started at the address: %X", i2cAddr);
        Serial.print(buffer);
        showAllRegisters();
        mcp.turnGpioOn(2); // Sets the GOPI2 high
        mcp.turnGpioOn(4);
        showAllRegisters();
        mcp.turnGpioOff(2); // Sets the GPIO2 low
        showAllRegisters();

        // Sets the GPIO pins 0,2,3 and 7 to input and 1, 4, 5 and 6 to output
        mcp.setRegister(REG_IODIR, 0B10001101);
        showAllRegisters();
        // Sets pull-up resistors to gpio pins 0, 2, 3 and 7
        mcp.pullUpGpioOn(0);
        mcp.pullUpGpioOn(2);
        mcp.pullUpGpioOn(3);
        mcp.pullUpGpioOn(7);
        showAllRegisters();
        // You also can use mcp.setRegister(REG_GPPU, 0B10001101); // All at once   
    } else {
        Serial.print("\nAny MCP23008 device was found!");
        while(1);
    }

}

uint8_t getI2cAddress()
{
    int err = 0;
    Wire.begin();
    for (int addr = 0x20; addr <= 0x27; addr++)
    {
        Wire.beginTransmission(addr);
        err = Wire.endTransmission();
        if (err == 0)
            return addr;
    }
    // Any MCP23008 device was found
    return 0;
}


void showAllRegisters() {
   Serial.print("\n********************\nCurrent register values\n");  
   for (uint8_t reg = REG_IODIR; reg <=REG_OLAT; reg++ ) {
         sprintf(buffer,"\nRegister 0x%X = ", reg);
         Serial.print(buffer);
         Serial.print(mcp.getRegister(reg), BIN);   
   }
}

void loop () {
    
}
