#define _MODLIB_SLAVE_INPUT_REGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern void MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern void MODBUSParseRequest04( union MODBUSParser *Parser );

//Functions for building responses
extern void MODBUSBuildResponse04( union MODBUSParser *Parser );
