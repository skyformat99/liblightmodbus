#define _LIGHTMODBUS_MASTER_TYPES

#include <inttypes.h>

#ifndef _LIGHTMODBUS_BASE
#include "../core.h"
#endif

//Declarations for master types
typedef enum
{
	Register = 0,
	Coil = 1,
	DiscreteInput = 2,
	InputRegister = 3
} MODBUSDataType_t; //MODBUS data types enum (coil, register, input, etc.)

typedef struct
{
	uint8_t Address; //Device address
	uint8_t Function; //Function called, in which exception occured
	uint8_t Code; //exception code
} MODBUSException_t; //Parsed exception data

typedef struct
{
	uint8_t Address; //Device address
	MODBUSDataType_t DataType; //Data type
	uint16_t Register; //Register, coil, input ID
	uint16_t Value; //Value of data
} MODBUSData_t;

typedef struct
{
	MODBUSData_t *Data; //Data read from slave
	uint8_t DataLength; //Count of data type instances read from slave
	uint8_t Finished; //Is parsing finished?
	MODBUSException_t exception; //Optional exception read
	MODBUSFrame_t Request; //Formatted request for slave
} MODBUSMasterStatus_t; //Type containing master device configuration data
