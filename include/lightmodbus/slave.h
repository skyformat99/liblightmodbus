#define _LIGHTMODBUS_SLAVE_BASE

#include <inttypes.h>

#ifndef _LIGHTMODBUS_BASE
#include "core.h"
#endif

#ifndef _LIGHTMODBUS_PARSER
#include "parser.h"
#endif

#include "slave/stypes.h"
#include "slave/sregisters.h"
#include "slave/scoils.h"
#include "slave/sdiscreteinputs.h"
#include "slave/sinputregisters.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef LIGHTMODBUS_SLAVE_REGISTERS
#define LIGHTMODBUS_SLAVE_REGISTERS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_COILS
#define LIGHTMODBUS_SLAVE_COILS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_DISCRETE_INPUTS
#define LIGHTMODBUS_SLAVE_DISCRETE_INPUTS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_INPUT_REGISTERS
#define LIGHTMODBUS_SLAVE_INPUT_REGISTERS 0
#endif

//Variables declarations
extern MODBUSSlaveStatus_t MODBUSSlave; //Slave configuration

//Function prototypes
extern uint8_t MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern uint8_t MODBUSSlaveInit( uint8_t ); //Very basic init of slave side
extern void MODBUSSlaveDestroy( ); //Free memory used by slave
