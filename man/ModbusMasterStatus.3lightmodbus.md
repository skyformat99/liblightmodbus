# ModbusMasterStatus 3lightmodbus "28 July 2016" "v1.2"

## NAME
**ModbusMasterStatus** - data type containing all information about current master device status and its configuration.

## SYNOPSIS
`typedef struct
	{
		ModbusData *data; //Data read from slave
		uint8_t dataLength; //Count of data type instances read from slave
		uint8_t finished; //Is parsing finished?
		ModbusException exception; //Optional exception read
		ModbusFrame request; //Formatted request for slave
	} ModbusMasterStatus; //Type containing master device configuration data`

## DESCRIPTION
The **ModbusMasterStatus** contains information about master device configuration and status. To make sure, that structure is set up for use properly,
remember to call **modbusMasterInit**.

| member name    | description                                                                                          |
|----------------|------------------------------------------------------------------------------------------------------|
| data           | dynamically allocated array of type **ModbusData**, and length of *dataLength* containing data read from salve device |
| dataLength     | length of *data* array                                                                               |
| finished       | If not equal to 0, data processing has finished, and results can be read. Useful when multiple threads are present. |
| exception      | **ModbusException** structure, containing information about exception returned by slave, if any      |
| request        | **ModbusFrame** structure, containing request frame built by master device, after one of **modbusBuildRequest** functions was called |

## NOTES
**ModbusMasterStatus** is declared in **lightmodbus/master/mtypes.h**, although including **lightmodbus/master.h** is enough.

## SEE ALSO
ModbusFrame( 3lightmodbus ), ModbusException( 3lightmodbus ), ModbusData( 3lightmodbus ), modbusMasterInit( 3lightmodbus ), modbusMasterEnd( 3lightmodbus )
