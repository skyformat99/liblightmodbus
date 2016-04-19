#define _SLAVEREGISTERS

#include <inttypes.h>

//Functions needed from other modules
extern void MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern void MODBUSParseRequest03( union MODBUSParser *Parser );
extern void MODBUSParseRequest06( union MODBUSParser *Parser );
extern void MODBUSParseRequest16( union MODBUSParser *Parser );

//Functions for building responses
extern void MODBUSBuildResponse03( union MODBUSParser *Parser );
extern void MODBUSBuildResponse06( union MODBUSParser *Parser );
extern void MODBUSBuildResponse16( union MODBUSParser *Parser );
