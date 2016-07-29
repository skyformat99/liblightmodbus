# ModbusData 3lightmodbus "28 July 2016" "v1.2"

## NAME
**ModbusData** - data type containing single Modbus data unit (e.g. register, coil).

## SYNOPSIS
`typedef struct
	{
		uint8_t address; //Device address
		ModbusDataType dataType; //Data type
		uint16_t reg; //Register, coil, input ID
		uint16_t value; //Value of data
	} ModbusData;`

## DESCRIPTION
The **ModbusData** contains single Modbus data unit (e.g. register, coil) and information about it.

| member name    | description                                                                                          |
|----------------|------------------------------------------------------------------------------------------------------|
| address        | address of slave that sent this data (useful, when multiple slave devices are connected)             |
| dataType       | **ModbusDataType** data type enumerator                                                              |
| reg            | register/coil index                                                                                  |
| value          | value of register/coil                                                                               |

## NOTES
**ModbusData** is declared in **lightmodbus/master/mtypes.h**, although including **lightmodbus/master.h** is enough.
No multiple coil types are stored in the same instance of **ModbusData**. Each coil type read is stored in another structure for simplicity.

## SEE ALSO
ModbusMasterStatus( 3lightmodbus ), ModbusDataType( 3lightmodbus )
