#define _MASTERBASIC

#include <inttypes.h>

//Basic master parser
extern uint8_t MODBUSParseResponseBasic( union MODBUSParser *, union MODBUSParser * );

//Functions for creating requests
void MODBUSBuildRequest06( uint8_t, uint16_t, uint16_t );
