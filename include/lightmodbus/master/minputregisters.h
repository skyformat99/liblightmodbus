#ifndef LIGHTMODBUS_MASTER_INPUT_REGISTERS_H
#define LIGHTMODBUS_MASTER_INPUT_REGISTERS_H

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest04( uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern uint8_t modbusParseResponse04( union ModbusParser *, union ModbusParser * );

#endif
