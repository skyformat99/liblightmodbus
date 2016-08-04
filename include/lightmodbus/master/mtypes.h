#ifndef LIGHTMODBUS_MASTER_TYPES_H
#define LIGHTMODBUS_MASTER_TYPES_H

#include <inttypes.h>
#include "../core.h"

//Declarations for master types
typedef struct
{
	uint8_t address; //Device address
	uint8_t function; //In which function exception occured
	uint8_t code; //Exception code
} ModbusException; //Parsed exception data

typedef enum //MODBUS data types enum (coil, reg, input, etc.)
{
	holdingRegister = 0,
	inputRegister = 1,
	coil = 2,
	discreteInput = 4
} ModbusDataType;

typedef struct
{
	uint8_t address; //Device address
	ModbusDataType dataType; //Data type
	uint16_t reg; //Register, coil, input ID
	uint16_t value; //Value of data
} ModbusData;

typedef struct
{
	ModbusData *data; //Data read from slave
	uint8_t dataLength; //Count of data type instances read from slave
	uint8_t finished; //Is parsing finished?
	ModbusException exception; //Optional exception read
	ModbusFrame request; //Formatted request for slave
	ModbusFrame response; //Response from slave should be put here
} ModbusMasterStatus; //Type containing master device configuration data

#endif
