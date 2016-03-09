#include "modlib.h"
#include "parser.h"
#include "exception.h"

//Types
typedef struct
{
	uint8_t Length; //Length of frame, if it's equal to 0, frame is not ready
	uint8_t *Frame; //Response frame content
} MODBUSResponseStatus; //Type containing information about frame that is set up at slave side

typedef struct
{
	uint8_t Address;
	uint16_t *Registers;
	uint16_t RegisterCount;
	MODBUSResponseStatus Response;
} MODBUSSlaveStatus; //Type containing slave device configuration data

//Variables definitions
extern MODBUSSlaveStatus MODBUSSlave; //Slave configuration

//Function prototypes
extern void MODBUSException( uint8_t, uint8_t ); //Generate exception response to response frame buffer
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern void MODBUSSlaveInit( uint8_t, uint16_t *, uint16_t ); //Very basic init of slave side
