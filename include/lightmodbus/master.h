#ifndef LIGHTMODBUS_MASTER_BASE_H
#define LIGHTMODBUS_MASTER_BASE_H

#include <inttypes.h>

#include "lightmodbus/master/mtypes.h"

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

extern uint8_t modbusParseResponse( ModbusMasterStatus *status, uint8_t *, uint8_t, uint8_t *, uint8_t );
extern uint8_t modbusMasterInit( ModbusMasterStatus *status );
extern void modbusMasterEnd( ModbusMasterStatus *status ); //Free memory used by master

#endif
