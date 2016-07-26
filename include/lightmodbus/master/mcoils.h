#ifndef LIGHTMODBUS_MASTER_COILS_H
#define LIGHTMODBUS_MASTER_COILS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest01( ModbusMasterStatus *status, uint8_t address, uint16_t firstCoil, uint16_t coilCount );
extern uint8_t modbusBuildRequest05( ModbusMasterStatus *status, uint8_t address, uint16_t coil, uint16_t value );
extern uint8_t modbusBuildRequest15( ModbusMasterStatus *status, uint8_t address, uint16_t firstCoil, uint16_t coilCount, uint8_t *values );

//Functions for parsing responses
extern uint8_t modbusParseResponse01( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
extern uint8_t modbusParseResponse05( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
extern uint8_t modbusParseResponse15( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );

#endif
