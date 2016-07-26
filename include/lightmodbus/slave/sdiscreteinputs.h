#ifndef LIGHTMODBUS_SLAVE_DISCRETE_INPUTS_H
#define LIGHTMODBUS_SLAVE_DISCRETE_INPUTS_H

#include <inttypes.h>
#include "stypes.h"

//Functions needed from other modules
extern uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t function, uint8_t exceptionCode );

//Functions for parsing requests
extern uint8_t modbusParseRequest02( ModbusSlaveStatus *status, union ModbusParser *parser );

#endif
