# ModbusDataType 3LIGHTMODBUS "28 July 2016" "v1.2"

## NAME
**ModbusDataType** - enumeration type, describing what type Modbus data is (e.g. coil, register).

## SYNOPSIS
`typedef enum //MODBUS data types enum (coil, reg, input, etc.)
	{
		holdingRegister = 0,
		inputRegister = 1,
		coil = 2,
		discreteInput = 4
	} ModbusDataType;`

## DESCRIPTION
The **ModbusDataType** describes what type given Modbus data is.

| value | data type                 |
|-------|---------------------------|
| 0     | holding register (16-bit) |
| 1     | input register (16-bit)   |
| 2     | coil (1-bit)              |
| 4     | discrete input (1-bit)    |


## NOTES
**ModbusDataType** is declared in **lightmodbus/master/mtypes.h**, although including **lightmodbus/master.h** is enough.

## SEE ALSO
ModbusMasterStatus( 3LIGHTMODBUS ), ModbusData( 3LIGHTMODBUS )
