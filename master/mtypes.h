#define _MODLIB_MASTER_TYPES

#include <inttypes.h>

//Declarations for master types

typedef enum
{
	Register = 0,
	Coil = 1,
	DiscreteInput = 2,
	InputRegister = 3
} MODBUSDataType; //MODBUS data types enum (coil, register, input, etc.)

typedef struct
{
	uint8_t Address; //Device address
	uint8_t Function; //Function called, in which exception occured
	uint8_t Code; //Exception code
} MODBUSException; //Parsed exception data

typedef struct
{
	uint8_t Address; //Device address
	MODBUSDataType DataType; //Data type
	uint16_t Register; //Register, coil, input ID
	uint16_t Value; //Value of data
} MODBUSData;

typedef struct
{
	uint8_t Length; //Length of frame, if it's equal to 0, frame is not ready
	uint8_t *Frame; //Request frame content
} MODBUSRequestStatus; //Type containing information about frame that is set up at master side

typedef struct
{
	MODBUSData *Data; //Data read from slave
	MODBUSException Exception; //Optional exception read
	MODBUSRequestStatus Request; //Formatted request for slave
	uint8_t DataLength; //Count of data type instances read from slave
	uint8_t Error; //Have any error occured?
	uint8_t Finished; //Is parsing finished?
} MODBUSMasterStatus; //Type containing master device configuration data
