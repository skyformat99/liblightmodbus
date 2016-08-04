# ModbusMasterStatus 3lightmodbus "28 July 2016" "v1.2"

## NAME
**ModbusMasterStatus** - data type containing all information about current master device status and its configuration.

## SYNOPSIS
`  
	typedef struct
	{
		ModbusData *data; //Data read from slave
		uint8_t dataLength; //Count of data read from slave
		uint8_t finished; //Is parsing finished?
		ModbusException exception; //Optional exception read
		ModbusFrame request; //Formatted request for slave
		ModbusFrame response; //Response from slave
	} ModbusMasterStatus; //Master device configuration
`

## DESCRIPTION
The **ModbusMasterStatus** contains information about master device configuration and status. To make sure, that structure is set up for use properly,
remember to call **modbusMasterInit**.

| member name  | description                                                  |
|--------------|--------------------------------------------------------------|
| `data`       | data from slave device                                       |
| `dataLength` | length of *data* array                                       |
| `finished`   | has processing finished?                                     |
| `exception`  | information about exception returned by slave                |
| `request`    | request frame                                                |
| `response`   | response frame from slave should be put here                 |

`data` points to dynamically allocated array of type **ModbusData**, and length of `dataLength` containing data read from salve device.

`finished` is set to non-zero value, when parsing frame is finished, and results are available.

`exception` contains exception information, if any.

`request` contains request frame, ought to be send to slave device.

`response` should contain response frame from slave.

## NOTES
**ModbusMasterStatus** is declared in **lightmodbus/master/mtypes.h**, although including **lightmodbus/master.h** is enough.

## SEE ALSO
ModbusFrame( 3lightmodbus ), ModbusException( 3lightmodbus ), ModbusData( 3lightmodbus ), modbusMasterInit( 3lightmodbus ), modbusMasterEnd( 3lightmodbus )
