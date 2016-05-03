#define _MODLIB_BASE

/* This is main header file that is ought to be included as library */

//Include proper header files
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

//Error codes
#define MODBUS_ERROR_PARSE 1 //Slave did not parse frame (eg. bad function code)
#define MODBUS_ERROR_EXCEPTION 2 //Slave thrown an exception and it's stored in MODBUSMaster.Exception
#define MODBUS_ERROR_CRC 4 //Invalid CRC error
#define MODBUS_ERROR_ALLOC 8 //Memory allocation problems (eg. system ran out of RAM)
#define MODBUS_ERROR_OTHER 16 //Other reason function was exited (eg. bad function parameter)
#define MODBUS_ERROR_FRAME 32 //Frame contained incorrect data, and exception could not be thrown (eg. bytes count != register count * 2 in slave's response)

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
