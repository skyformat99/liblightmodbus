#define _LIGHTMODBUS_SLAVE_INPUT_REGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t modbusBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t modbusParseRequest04( union ModbusParser *parser );
