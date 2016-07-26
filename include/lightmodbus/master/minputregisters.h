#ifndef LIGHTMODBUS_MASTER_INPUT_REGISTERS_H
#define LIGHTMODBUS_MASTER_INPUT_REGISTERS_H

#include <inttypes.h>
#include "lightmodbus/master/mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest04( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern uint8_t modbusParseResponse04( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );

#endif
