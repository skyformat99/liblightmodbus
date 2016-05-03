#define _MODLIB_SLAVE_REGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t MODBUSParseRequest03( union MODBUSParser *Parser );
extern uint8_t MODBUSParseRequest06( union MODBUSParser *Parser );
extern uint8_t MODBUSParseRequest16( union MODBUSParser *Parser );

//Functions for building responses
extern uint8_t MODBUSBuildResponse03( union MODBUSParser *Parser );
extern uint8_t MODBUSBuildResponse06( union MODBUSParser *Parser );
extern uint8_t MODBUSBuildResponse16( union MODBUSParser *Parser );
