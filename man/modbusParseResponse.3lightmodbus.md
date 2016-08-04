# modbusParseResponse 3lightmodbus "4 August 2016" "v1.2"

## NAME
**modbusParseResponse**, **modbusParseResponse01**, **modbusParseResponse02**, **modbusParseResponse03**, **modbusParseResponse04**, **modbusParseResponse05**, **modbusParseResponse06**, **modbusParseResponse15**, **modbusParseResponse16** - parse response frame returned by slave device.

## SYNOPSIS
`#include <lightmodbus/master.h>`

`  
	uint8_t modbusParseResponse( ModbusMasterStatus *status );
	uint8_t modbusParseResponse01( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse02( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse03( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse04( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse05( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse06( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse15( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
	uint8_t modbusParseResponse16( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser );
`

## DESCRIPTION
The **modbusParseResponse** function parses request frame located in *status.response*. Results are written into *status.data*, and *status.finished* is set to 1, when function exits.
Also, an error code is returned (described in lightmodbus(3lightmodbus)).

**modbusParseResponse01**, **modbusParseResponse02**, and so on can only parse specific function responses, while **modbusParseResponse** automatically picks on of them. Keep in mind, that calling them directly is unsafe.

## SEE ALSO
lightmodbus(3lightmodbus), ModbusMasterStatus(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
