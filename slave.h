#define _MODLIB_SLAVE_BASE

#include <inttypes.h>

#ifndef _MODLIB_BASE
#include "modlib.h"
#endif

#ifndef _MODLIB_PARSER
#include "parser.h"
#endif

#include "slave/stypes.h"
#include "slave/sregisters.h"
#include "slave/scoils.h"
#include "slave/sdiscreteinputs.h"
#include "slave/sinputregisters.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef MODBUS_SLAVE_REGISTERS
#define MODBUS_SLAVE_REGISTERS 0
#endif
#ifndef MODBUS_SLAVE_COILS
#define MODBUS_SLAVE_COILS 0
#endif
#ifndef MODBUS_SLAVE_DISCRETE_INPUTS
#define MODBUS_SLAVE_DISCRETE_INPUTS 0
#endif
#ifndef MODBUS_SLAVE_INPUT_REGISTERS
#define MODBUS_SLAVE_INPUT_REGISTERS 0
#endif

//Variables declarations
extern MODBUSSlaveStatus MODBUSSlave; //Slave configuration

//Function prototypes
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern void MODBUSSlaveInit( uint8_t ); //Very basic init of slave side
