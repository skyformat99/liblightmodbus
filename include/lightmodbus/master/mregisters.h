#ifndef LIGHTMODBUS_MASTER_REGISTERS_H
#define LIGHTMODBUS_MASTER_REGISTERS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest03( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount );
extern uint8_t modbusBuildRequest06( ModbusMasterStatus *status, uint8_t address, uint16_t reg, uint16_t value );
extern uint8_t modbusBuildRequest16( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount, uint16_t *values );

//Functions for parsing responses
extern uint8_t modbusParseResponse03( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
extern uint8_t modbusParseResponse06( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
extern uint8_t modbusParseResponse16( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );

#endif
