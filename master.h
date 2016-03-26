#define _MASTERBASE

#include <inttypes.h>

#ifndef _MODLIB
#include "modlib.h"
#endif

#ifndef _MODBUS_PARSER
#include "parser.h"
#endif

#include "master/mtypes.h"
#include "master/mbasic.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef MODBUS_MASTER_SUPPORT
#define MODBUS_MASTER_SUPPORT 0
#endif

//Variables declarations
extern MODBUSMasterStatus MODBUSMaster; //Master configuration

extern uint8_t MODBUSParseResponse( uint8_t *, uint8_t, uint8_t *, uint8_t );
extern void MODBUSMasterInit( );
