#ifndef LIGHTMODBUS_SLAVE_TYPES_H
#define LIGHTMODBUS_SLAVE_TYPES_H

#include <inttypes.h>
#include "lightmodbus/core.h"

//Declarations for slave types

typedef struct
{
	uint8_t address; //Slave address

	uint16_t *Registers; //Slave holding registers
	uint16_t registerCount; //Slave reg count

	uint8_t *Coils; //Slave coils
	uint16_t coilCount; //Slave coil count

	uint8_t *DiscreteInputs; //Slave discrete input
	uint16_t DiscreteInputCount; //Slave discrete input count

	uint8_t *RegisterMask; //Masks for write protection (bit of value 1 - write protection)
	uint16_t RegisterMaskLength; //Masks length (each mask covers 8 registers)

	uint16_t *InputRegisters; //Slave input registers
	uint16_t InputRegisterCount; //Slave input count

	uint8_t finished; //Has slave finished building response for master?
	ModbusFrame response; //Slave response formatting status
} ModbusSlaveStatus; //Type containing slave device configuration data

#endif
