#define _SLAVEBASE

#include <inttypes.h>

#ifndef _MODLIB
#include "modlib.h"
#endif

#ifndef _MODBUS_PARSER
#include "parser.h"
#endif

#include "slave/stypes.h"
#include "slave/sbasic.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef MODBUS_SLAVE_SUPPORT
#define MODBUS_SLAVE_SUPPORT 0
#endif

//Variables declarations
extern MODBUSSlaveStatus MODBUSSlave; //Slave configuration

//Function prototypes
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern void MODBUSSlaveInit( uint8_t, uint16_t *, uint16_t ); //Very basic init of slave side
