#define _SLAVEBASIC

#include <inttypes.h>

//Basic parser
extern uint8_t MODBUSParseRequestBasic( union MODBUSParser * );

//Functions needed from other modules
extern void MODBUSBuildException( uint8_t, uint8_t );
