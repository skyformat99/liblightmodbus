#define _SLAVECOILS

#include <inttypes.h>

//Functions for parsing requests
extern uint8_t MODBUSParseRequest01( union MODBUSParser * );
extern uint8_t MODBUSParseRequest05( union MODBUSParser * );
extern uint8_t MODBUSParseRequest15( union MODBUSParser * );

//Functions for building responses
//extern void MODBUSBuildResponse01( union MODBUSParser * );
//extern void MODBUSBuildResponse05( union MODBUSParser * );
//extern void MODBUSBuildResponse15( union MODBUSParser * );
