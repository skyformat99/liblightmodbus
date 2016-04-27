#define _MODLIB_MASTER_DISCRETE_INPUTS

#include <inttypes.h>

//Functions for building requests
extern uint8_t MODBUSBuildRequest02( uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern void MODBUSParseResponse02( union MODBUSParser *, union MODBUSParser * );
