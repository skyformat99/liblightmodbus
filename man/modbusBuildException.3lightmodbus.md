# modbusBuildException 3lightmodbus "4 August 2016" "v1.2"

## NAME
**modbusBuildException** - build Modbus exception frame.

## SYNOPSIS
`#include <lightmodbus/slave.h>`

`uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t function, uint8_t exceptionCode )`

## DESCRIPTION
The **modbusBuildException** function builds Modbus exception frame, later located in *status.response*.
Argument *function* should be the number of function that throws an exception. *exceptionCode* is the exception code to be thrown.
Function returns an error value on exit (described in lightmodbus(3lightmodbus)), and sets *status.finished* to 1.

## SEE ALSO
modbusParseException(3lightmodbus), ModbusException(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
