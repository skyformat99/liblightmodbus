#ifndef _MODLIB
#include "modlib.h"
#endif

#ifndef _MODBUS_PARSER
#include "parser.h"
#endif

#include "master/types.h"
#include <stdio.h> /*DEBUG*/

#include "master/basic.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef MODBUS_MASTER_BASIC
#define MODBUS_MASTER_BASIC 0
#endif

//Variables declarations
extern MODBUSMasterStatus MODBUSMaster; //Master configuration

extern void MODBUSParseResponse( uint8_t *, uint8_t );
extern void MODBUSMasterInit( );
