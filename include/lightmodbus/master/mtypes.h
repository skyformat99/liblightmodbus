#define _LIGHTMODBUS_MASTER_TYPES

#include <inttypes.h>

#ifndef _LIGHTMODBUS_BASE
#include "../core.h"
#endif

//Declarations for master types
typedef enum
{
	reg = 0,
	coil = 1,
	DiscreteInput = 2,
	InputRegister = 3
} MODBUSDataType_t; //MODBUS data types enum (coil, reg, input, etc.)

typedef struct
{
	uint8_t address; //Device address
	uint8_t function; //function called, in which exception occured
	uint8_t Code; //exception code
} MODBUSException_t; //Parsed exception data

typedef struct
{
	uint8_t address; //Device address
	MODBUSDataType_t DataType; //Data type
	uint16_t reg; //reg, coil, input ID
	uint16_t value; //value of data
} MODBUSData_t;

typedef struct
{
	MODBUSData_t *Data; //Data read from slave
	uint8_t DataLength; //Count of data type instances read from slave
	uint8_t Finished; //Is parsing finished?
	MODBUSException_t exception; //Optional exception read
	MODBUSFrame_t request; //Formatted request for slave
} MODBUSMasterStatus_t; //Type containing master device configuration data
