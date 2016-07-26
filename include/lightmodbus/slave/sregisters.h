#ifndef LIGHTMODBUS_SLAVE_REGISTERS_H
#define LIGHTMODBUS_SLAVE_REGISTERS_H

#include <inttypes.h>
#include "stypes.h"

//Functions needed from other modules
extern uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t modbusParseRequest03( ModbusSlaveStatus *status, union ModbusParser *parser );
extern uint8_t modbusParseRequest06( ModbusSlaveStatus *status, union ModbusParser *parser );
extern uint8_t modbusParseRequest16( ModbusSlaveStatus *status, union ModbusParser *parser );

#endif
