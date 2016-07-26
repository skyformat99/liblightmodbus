#ifndef LIGHTMODBUS_MASTER_COILS_H
#define LIGHTMODBUS_MASTER_COILS_H

#include <inttypes.h>
#include "mtypes.h"

//Functions for building requests
extern uint8_t modbusBuildRequest01( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest05( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest15( ModbusMasterStatus *status, uint8_t, uint16_t, uint16_t, uint8_t * );

//Functions for parsing responses
extern uint8_t modbusParseResponse01( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse05( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse15( ModbusMasterStatus *status, union ModbusParser *, union ModbusParser * );

#endif
