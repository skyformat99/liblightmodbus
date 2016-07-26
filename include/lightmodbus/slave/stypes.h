#ifndef LIGHTMODBUS_SLAVE_TYPES_H
#define LIGHTMODBUS_SLAVE_TYPES_H

#include <inttypes.h>
#include "../core.h"

//Declarations for slave types

typedef struct
{
	uint8_t address; //Slave address

	uint16_t *registers; //Slave holding registers
	uint16_t registerCount; //Slave register count

	uint8_t *coils; //Slave coils
	uint16_t coilCount; //Slave coil count

	uint8_t *discreteInputs; //Slave discrete input
	uint16_t discreteInputCount; //Slave discrete input count

	uint8_t *registerMask; //Masks for write protection (bit of value 1 - write protection)
	uint16_t registerMaskLength; //Masks length (each mask covers 8 registers)

	uint16_t *inputRegisters; //Slave input registers
	uint16_t inputRegisterCount; //Slave input count

	uint8_t finished; //Has slave finished building response for master?
	ModbusFrame response; //Slave response formatting status
} ModbusSlaveStatus; //Type containing slave device configuration data

#endif
