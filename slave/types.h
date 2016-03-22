//Declarations for slave types

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
