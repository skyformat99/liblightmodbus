#define _MODLIB_MASTER_BASE

#include <inttypes.h>

#ifndef _MODLIB
#include "modlib.h"
#endif

#ifndef _MODBUS_PARSER
#include "parser.h"
#endif

#include "master/mtypes.h"
#include "master/mregisters.h"
#include "master/mcoils.h"
#include "master/mdiscreteinputs.h"
#include "master/minputregisters.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef MODBUS_MASTER_REGISTERS
#define MODBUS_MASTER_REGISTERS 0
#endif
#ifndef MODBUS_MASTER_COILS
#define MODBUS_MASTER_COILS 0
#endif
#ifndef MODBUS_MASTER_DISCRETE_INPUTS
#define MODBUS_MASTER_DISCRETE_INPUTS 0
#endif
#ifndef MODBUS_MASTER_INPUT_REGISTERS
#define MODBUS_MASTER_INPUT_REGISTERS 0
#endif

//Variables declarations
extern MODBUSMasterStatus MODBUSMaster; //Master configuration

extern uint8_t MODBUSParseResponse( uint8_t *, uint8_t, uint8_t *, uint8_t );
extern void MODBUSMasterInit( );
