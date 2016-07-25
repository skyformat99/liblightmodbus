#define _LIGHTMODBUS_SLAVE_REGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t modbusBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t modbusParseRequest03( union ModbusParser *parser );
extern uint8_t modbusParseRequest06( union ModbusParser *parser );
extern uint8_t modbusParseRequest16( union ModbusParser *parser );
