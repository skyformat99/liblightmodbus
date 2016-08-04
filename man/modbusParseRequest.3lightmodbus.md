# modbusParseRequest 3lightmodbus "4 August 2016" "v1.2"

## NAME
**modbusParseRequest** - parse request frame sent in by master device.

## SYNOPSIS
`#include <lightmodbus/slave.h>`

`uint8_t modbusParseRequest( ModbusSlaveStatus *status );`

## DESCRIPTION
The **modbusParseRequest** function parses request frame located in *status.request*. Response is automatically written to *status.response*, unless request
was broadcast.
When finished, an error code is returned (described in lightmodbus(3lightmodbus)).

## SEE ALSO
lightmodbus(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) <mrjjot@gmail.com>
