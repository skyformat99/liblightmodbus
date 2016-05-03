#define _MODLIB_SLAVE_INPUT_REGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t MODBUSParseRequest04( union MODBUSParser *Parser );

//Functions for building responses
extern uint8_t MODBUSBuildResponse04( union MODBUSParser *Parser );
