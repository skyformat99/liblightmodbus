#define _MASTERCOILS

#include <inttypes.h>

//Functions for building requests
extern uint8_t MODBUSBuildRequest01( uint8_t, uint16_t, uint16_t );
extern uint8_t MODBUSBuildRequest05( uint8_t, uint16_t, uint16_t );
extern uint8_t MODBUSBuildRequest15( uint8_t, uint16_t, uint16_t, uint8_t * );

//Functions for parsing responses
extern void MODBUSParseResponse01( union MODBUSParser *, union MODBUSParser * );
extern void MODBUSParseResponse05( union MODBUSParser *, union MODBUSParser * );
extern void MODBUSParseResponse15( union MODBUSParser *, union MODBUSParser * );
