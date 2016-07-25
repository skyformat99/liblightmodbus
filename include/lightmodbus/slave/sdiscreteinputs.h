#define _LIGHTMODBUS_SLAVE_DISCRETE_INPUTS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t modbusBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t MODBUSParseRequest02( union MODBUSParser * );
