# modbusParseException 3lightmodbus "28 July 2016" "v1.2"

## NAME
**modbusParseException** - parse exception frame returned by slave device.

## SYNOPSIS
`#include <lightmodbus/master.h>`

`uint8_t modbusParseException( ModbusMasterStatus *status, union ModbusParser *parser );`

## DESCRIPTION
The **modbusParseException** function parses exception frame stored in *parser* union, and leaves results in *status*.
If no error occurs, 0 value is returned.

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
