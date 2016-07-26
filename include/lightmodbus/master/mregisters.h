#ifndef LIGHTMODBUS_MASTER_REGISTERS_H
#define LIGHTMODBUS_MASTER_REGISTERS_H

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest03( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest06( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest16( uint8_t, uint16_t, uint16_t, uint16_t * );

//Functions for parsing responses
extern uint8_t modbusParseResponse03( union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse06( union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse16( union ModbusParser *, union ModbusParser * );

#endif
