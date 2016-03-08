/* This is main header file that is ought to be included as library */

//Include proper header files
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

//Function prototypes
extern uint16_t MODBUSSwapEndian( uint16_t );
extern uint16_t MODBUSCRC16( uint8_t *, uint16_t );
