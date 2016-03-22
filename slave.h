#include "modlib.h"
#include "parser.h"
#include "exception.h"
#include "slave/types.h"

#include "slave/basic.h"

//Enabling modules in compilation process
#ifndef MODBUS_SLAVE_BASIC
#define MODBUS_SLAVE_BASIC 0
#endif

//Variables declarations
extern MODBUSSlaveStatus MODBUSSlave; //Slave configuration

//Function prototypes
extern void MODBUSException( uint8_t, uint8_t ); //Generate exception response to response frame buffer
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern void MODBUSSlaveInit( uint8_t, uint16_t *, uint16_t ); //Very basic init of slave side
