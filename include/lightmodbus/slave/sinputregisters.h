#ifndef LIGHTMODBUS_SLAVE_INPUT_REGISTERS_H
#define LIGHTMODBUS_SLAVE_INPUT_REGISTERS_H

#include <inttypes.h>
#include "lightmodbus/slave/stypes.h"

//Functions needed from other modules
extern uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t modbusParseRequest04( ModbusSlaveStatus *status, union ModbusParser *parser );

#endif
