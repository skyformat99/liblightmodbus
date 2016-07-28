# ModbusSlaveStatus 3LIGHTMODBUS "28 July 2016" "v1.2"

## NAME
**ModbusSlaveStatus** - data type containing all information about current slave device status and its configuration.

## SYNOPSIS
`typedef struct
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
	} ModbusSlaveStatus; //Type containing slave device configuration data`

## DESCRIPTION
The **ModbusSlaveStatus** contains information about slave device configuration and status. To make sure, that structure is set up for use properly,
remember to call **modbusSlaveInit**. Values like **registers**, **registerCount**, etc. are ought to be set beforehand though.

| member name       | description                                                                                          |
|-------------------|------------------------------------------------------------------------------------------------------|
| address           | slave device address                                                                                 |
| registers         | holding registers array                                                                              |
| registerCount     | length of holding registers array                                                                    |
| coils             | coils array (each *bit* corresponds to one coil)                                                     |
| coilCount         | number of coils (not the length of **coils** array)                                                  |
| discreteInputs    | discrete inputs array (each *bit* corresponds to one input)                                          |
| discreteInputCount| number of discrete inputs (not the length of **discreteInputs** array)                               |
| inputRegisters    | input registers array                                                                                |
| inputRegisterCount| length of input registers array                                                                      |
| finished          | has slave finished parsing request and formatted response (useful when multiple threads are present) |
| response          | **ModbusFrame** structure, containing response frame for master device                               |

## NOTES
**ModbusSlaveStatus** is declared in **lightmodbus/slave/stypes.h**, although including **lightmodbus/slave.h** is enough.

## SEE ALSO
ModbusFrame( 3LIGHTMODBUS ), modbusSlaveInit( 3LIGHTMODBUS ), modbusSlaveEnd( 3LIGHTMODBUS )
