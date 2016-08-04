# ModbusSlaveStatus 3lightmodbus "28 July 2016" "v1.2"

## NAME
**ModbusSlaveStatus** - data type containing all information about current slave device status and its configuration.

## SYNOPSIS
`  
	typedef struct
	{
		uint8_t address; //Slave address
		uint16_t *registers; //Slave holding registers
		uint16_t registerCount; //Slave register count
		uint8_t *coils; //Slave coils
		uint16_t coilCount; //Slave coil count
		uint8_t *discreteInputs; //Slave discrete input
		uint16_t discreteInputCount; //Slave discrete input count
		uint8_t *registerMask; //Masks for write protection
		uint16_t registerMaskLength; //Masks length
		uint16_t *inputRegisters; //Slave input registers
		uint16_t inputRegisterCount; //Slave input count
		uint8_t finished; //Has slave finished building response?
		ModbusFrame response; //Slave response formatting status
		ModbusFrame request; //Request frame from master
	} ModbusSlaveStatus; //Slave device configuration data
`

## DESCRIPTION
The **ModbusSlaveStatus** contains information about slave device configuration and status. To make sure, that structure is set up for use properly,
remember to call **modbusSlaveInit**. Values like *registers*, *registerCount*, etc. are ought to be set beforehand though.

| member name         | description                                               |
|---------------------|-----------------------------------------------------------|
| `address`           | slave device address                                      |
| `registers`         | holding registers array                                   |
| `registerCount`     | length of holding registers array                         |
| `coils`             | coils array                                               |
| `coilCount`         | number of coils                                           |
| `discreteInputs`    | discrete inputs array                                     |
| `discreteInputCount`| number of discrete inputs                                 |
| `inputRegisters`    | input registers array                                     |
| `inputRegisterCount`| length of input registers array                           |
| `finished`          | has processing finished                                   |
| `response`          | response frame for master device                          |
| `request`           | request frame from master                                 |

## NOTES
**ModbusSlaveStatus** is declared in **lightmodbus/slave/stypes.h**, although including **lightmodbus/slave.h** is enough.
In *coils* and *discreteInputs* each bit matches one input/output, and
*discreteInputCount* and *coilCount* correspond to actual input/output count, not the array length!
When some request-parsing function is called, make sure that valid frame pointer is set inside *request*.
After setting structure up manually, **modbusSlaveInit** should be called.

Holding registers can be write-protected. To achieve that, accordingly set **bits** to 1 in *registerMask* array (of *registerMaskLength*).
For example, setting 17th bit to 1, will result in 17th register being read-only.
To write and read masks more easily see modbusMaskRead(3lightmodbus) and modbusMaskWrite(3lightmodbus).

Important thing is, *request* is not an array, just a pointer. **It does not point to allocated memory by default!**
Please, simply put address of your data there, and do not attempt copying it.

## SEE ALSO
ModbusFrame(3lightmodbus), modbusSlaveInit(3lightmodbus), modbusSlaveEnd(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
