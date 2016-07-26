#ifndef LIGHTMODBUS_MASTER_TYPES_H
#define LIGHTMODBUS_MASTER_TYPES_H

#include <inttypes.h>
#include "../core.h"

//Declarations for master types
typedef struct
{
	uint8_t address; //Device address
	uint8_t function; //In which function exception occured
	uint8_t Code; //Exception code
} ModbusException; //Parsed exception data

typedef struct
{
	uint8_t address; //Device address
	enum //MODBUS data types enum (coil, reg, input, etc.)
	{
		holdingRegister = 0,
		inputRegister = 3,
		coil = 1,
		discreteInput = 2
	} dataType;
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
} ModbusMasterStatus; //Type containing master device configuration data

#endif
