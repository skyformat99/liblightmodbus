#ifndef LIGHTMODBUS_MASTER_INPUT_REGISTERS_H
#define LIGHTMODBUS_MASTER_INPUT_REGISTERS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest04( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount );

//Functions for parsing responses
extern uint8_t modbusParseResponse04( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );

#endif
