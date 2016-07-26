#ifndef LIGHTMODBUS_SLAVE_COILS_H
#define LIGHTMODBUS_SLAVE_COILS_H

#include <inttypes.h>
#include "stypes.h"

//Functions needed from other modules
extern uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t function, uint8_t exceptionCode );

//Functions for parsing requests
extern uint8_t modbusParseRequest01( ModbusSlaveStatus *status, union ModbusParser *parser );
extern uint8_t modbusParseRequest05( ModbusSlaveStatus *status, union ModbusParser *parser );
extern uint8_t modbusParseRequest15( ModbusSlaveStatus *status, union ModbusParser *parser );

#endif
