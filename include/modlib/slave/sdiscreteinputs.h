#define _MODLIB_SLAVE_DISCRETE_INPUTS

#include <inttypes.h>

//Functions needed from other modules
extern void MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern void MODBUSParseRequest02( union MODBUSParser * );

//Functions for building responses
extern void MODBUSBuildResponse02( union MODBUSParser * );
