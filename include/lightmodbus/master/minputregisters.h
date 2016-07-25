#define _LIGHTMODBUS_MASTER_INPUT_REGISTERS

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest04( uint8_t, uint16_t, uint16_t );

//Functions for parsing responses
extern uint8_t MODBUSParseResponse04( union MODBUSParser *, union MODBUSParser * );
