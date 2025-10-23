// communicating between 2 ucontrollers
// ucontroller 2 is using SPI to retrieve datafrom the temp and humidity sensors
// ucontroller 2 collects its data into 2 variables

float temp; //temp in C
uint_t humid; // humidity in percentage

// these variables need to be transmitted to ucontroller 1
//first we need to convert the 32 bit float from temp into bytes that can be transferred over UART


//this can be done by casting the value into 4 seperae uint8_t

float temp = 23.75f;

uint8_t byte0 = *((uint8_t*)&temp);
uint8_t byte1 = *((uint8_t*)&temp + 1);
uint8_t byte2 = *((uint8_t*)&temp + 2);
uint8_t byte3 = *((uint8_t*)&temp + 3);

//now we can transmit these bytes over UART

UART_Transmit(byte0);
UART_Transmit(byte1);
UART_Transmit(byte2);
UART_Transmit(byte3);

// on ucontroller1 side, the flaot can be restored by using bitwise operators:

// bytes recieved MSB-first: b3 b2 b1 b0

uint32_t u = ((uint32_t)b3 << 24) | ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b0;

float temp = *((float*)&u);

