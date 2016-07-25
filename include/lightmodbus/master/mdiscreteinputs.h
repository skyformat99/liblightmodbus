#define _LIGHTMODBUS_MASTER_DISCRETE_INPUTS

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest02( uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern uint8_t MODBUSParseResponse02( union MODBUSParser *, union MODBUSParser * );
