#ifndef LIGHTMODBUS_MASTER_DISCRETE_INPUTS_H
#define LIGHTMODBUS_MASTER_DISCRETE_INPUTS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest02( ModbusMasterStatus *status, uint8_t address, uint16_t firstInput, uint16_t inputCount );

//Functions for parsing responses
extern uint8_t modbusParseResponse02( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );

#endif
