#ifndef LIGHTMODBUS_MASTER_DISCRETE_INPUTS_H
#define LIGHTMODBUS_MASTER_DISCRETE_INPUTS_H

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest02( uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern uint8_t modbusParseResponse02( union ModbusParser *, union ModbusParser * );

#endif
