# ModbusException 3lightmodbus "28 July 2016" "v1.2"

## NAME
**ModbusExeption** - data type containing information about exception frame returned by Modbus slave device.

## SYNOPSIS
`  
	typedef struct
	{
		uint8_t address; //Device address
		uint8_t function; //In which function exception occured
		uint8_t code; //Exception code
	} ModbusException; //Parsed exception data
`

## DESCRIPTION
The **ModbusException** contains information about exception frame returned by Modbus slave device.

| member name    | description                                                                                          |
|----------------|------------------------------------------------------------------------------------------------------|
| address        | slave's address                                                                                      |
| function       | what function caused an exception to be thrown                                                       |
| code           | exception code described in Modbus standard                                                          |


## NOTES
**ModbusException** is declared in **lightmodbus/master/mtypes.h**, although including **lightmodbus/master.h** is enough.

## SEE ALSO
ModbusMasterStatus( 3lightmodbus )
