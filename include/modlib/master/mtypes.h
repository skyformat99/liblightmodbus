#define _MODLIB_MASTER_TYPES

#include <inttypes.h>

#ifndef _MODLIB_BASE
#include "../modlib.h"
#endif

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
	MODBUSData *Data; //Data read from slave
	uint8_t DataLength; //Count of data type instances read from slave
	uint8_t Finished; //Is parsing finished?
	MODBUSException Exception; //Optional exception read
	MODBUSFrame Request; //Formatted request for slave
} MODBUSMasterStatus; //Type containing master device configuration data
