#define _MODLIB_BASE

/* This is main header file that is ought to be included as library */

//Include proper header files
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

//Types
typedef struct
{
	uint8_t Length; //Length of frame, if it's equal to 0, frame is not ready
	uint8_t *Frame; //Frame content
} MODBUSFrame; //Type containing information about generated frame

//Function prototypes
extern uint8_t MODBUSReadMaskBit( uint8_t *Mask, uint16_t MaskLength, uint16_t Bit );
extern uint8_t MODBUSWriteMaskBit( uint8_t *Mask, uint16_t MaskLength, uint16_t Bit, uint8_t Value );
extern uint16_t MODBUSSwapEndian( uint16_t );
extern uint16_t MODBUSCRC16( uint8_t *, uint16_t );
