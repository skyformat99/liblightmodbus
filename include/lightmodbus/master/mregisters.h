#ifndef LIGHTMODBUS_MASTER_REGISTERS_H
#define LIGHTMODBUS_MASTER_REGISTERS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest03( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest06( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest16( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t, uint16_t * );

//Functions for parsing responses
extern uint8_t modbusParseResponse03( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse06( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse16( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );

#endif
