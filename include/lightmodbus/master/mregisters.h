#define _LIGHTMODBUS_MASTER_REGISTERS

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest03( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest06( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest16( uint8_t, uint16_t, uint16_t, uint16_t * );

//Functions for parsing responses
extern uint8_t MODBUSParseResponse03( union MODBUSParser *, union MODBUSParser * );
extern uint8_t MODBUSParseResponse06( union MODBUSParser *, union MODBUSParser * );
extern uint8_t MODBUSParseResponse16( union MODBUSParser *, union MODBUSParser * );
