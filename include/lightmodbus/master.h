#ifndef LIGHTMODBUS_MASTER_BASE_H
#define LIGHTMODBUS_MASTER_BASE_H

#include <inttypes.h>

#include "core.h"
#include "parser.h"
#include "master/mtypes.h"
#include "master/mregisters.h"
#include "master/mcoils.h"
#include "master/mdiscreteinputs.h"
#include "master/minputregisters.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef LIGHTMODBUS_MASTER_REGISTERS
#define LIGHTMODBUS_MASTER_REGISTERS 0
#endif
#ifndef LIGHTMODBUS_MASTER_COILS
#define LIGHTMODBUS_MASTER_COILS 0
#endif
#ifndef LIGHTMODBUS_MASTER_DISCRETE_INPUTS
#define LIGHTMODBUS_MASTER_DISCRETE_INPUTS 0
#endif
#ifndef LIGHTMODBUS_MASTER_INPUT_REGISTERS
#define LIGHTMODBUS_MASTER_INPUT_REGISTERS 0
#endif

extern uint8_t modbusParseException( ModbusMasterStatus *status, union ModbusParser *parser );
extern uint8_t modbusParseResponse( ModbusMasterStatus *status, uint8_t *frame, uint8_t frameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength );
extern uint8_t modbusMasterInit( ModbusMasterStatus *status );
extern void modbusMasterEnd( ModbusMasterStatus *status ); //Free memory used by master

#endif
