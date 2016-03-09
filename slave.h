#include "modlib.h"
#include "parser.h"
#include "exception.h"

//Types
typedef struct
{
	uint8_t Length; //Length of frame, if it's equal to 0, frame is not ready
	uint8_t *Frame; //Response frame content
} MODBUSResponseStatus; //Type containing information about frame that is set up at slave side

//Variables definitions
extern uint8_t MODBUSAddress; //Address of device
extern uint16_t *MODBUSRegisters; //Pointer to slave-side modbus registers
extern uint16_t MODBUSRegisterCount; //Count of slave-side modbus registers
extern MODBUSResponseStatus MODBUSSlaveResponse; //Struct containing response for master and its status

//Function prototypes
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern void MODBUSSlaveInit( uint8_t, uint16_t *, uint16_t ); //Very basic init of slave side
