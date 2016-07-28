# ModbusFrame 3LIGHTMODBUS "28 July 2016" "v1.2"

## NAME
**ModbusFrame** - data type containing Modbus frame and information about it.

## SYNOPSIS
`typedef struct
	{
		uint8_t length; //Length of frame, if it's equal to 0, frame is not ready
		uint8_t *frame; //Frame content
	} ModbusFrame; //Type containing information about generated frame`

## DESCRIPTION
The **ModbusFrame** contains pointer to frame data, and frame length expressed in bytes number.

| member name    | description                                                                                          |
|----------------|------------------------------------------------------------------------------------------------------|
| length         | dynamically allocated array of type **uint8_t**, and length of *length* containing Modbus frame      |
| frame          | length of *frame* in bytes                                                                           |


## NOTES
**ModbusFrame** is declared in **lightmodbus/core.h**.
Also, maximum length of single frame is 256 bytes, as limited by 8-bit unsigned integer and Modbus standard.

## SEE ALSO
ModbusMasterStatus( 3LIGHTMODBUS )
