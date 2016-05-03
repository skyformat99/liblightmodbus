#define _MODLIB_SLAVE_COILS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t MODBUSBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t MODBUSParseRequest01( union MODBUSParser * );
extern uint8_t MODBUSParseRequest05( union MODBUSParser * );
extern uint8_t MODBUSParseRequest15( union MODBUSParser * );

//Functions for building responses
extern uint8_t MODBUSBuildResponse01( union MODBUSParser * );
extern uint8_t MODBUSBuildResponse05( union MODBUSParser * );
extern uint8_t MODBUSBuildResponse15( union MODBUSParser * );
