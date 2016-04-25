#define _SLAVECOILS

#include <inttypes.h>

//Functions needed from other modules
extern void MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern void MODBUSParseRequest01( union MODBUSParser * );
extern void MODBUSParseRequest05( union MODBUSParser * );
extern void MODBUSParseRequest15( union MODBUSParser * );

//Functions for building responses
extern void MODBUSBuildResponse01( union MODBUSParser * );
extern void MODBUSBuildResponse05( union MODBUSParser * );
extern void MODBUSBuildResponse15( union MODBUSParser * );
