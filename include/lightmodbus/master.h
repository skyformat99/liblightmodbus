#define _LIGHTMODBUS_MASTER_BASE

#include <inttypes.h>

#ifndef _LIGHTMODBUS_BASE
#include "core.h"
#endif

#ifndef _LIGHTMODBUS_PARSER
#include "parser.h"
#endif

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

//Variables declarations
extern ModbusMasterStatus MODBUSMaster; //Master configuration

extern uint8_t modbusParseResponse( uint8_t *, uint8_t, uint8_t *, uint8_t );
extern uint8_t modbusMasterInit( );
extern void modbusMasterEnd( ); //Free memory used by master
