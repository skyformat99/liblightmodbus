# modbusSlaveEnd 3lightmodbus "29 July 2016" "v1.2"

## NAME
**modbusSlaveEnd** - free memory used in **ModbusSlaveStatus** structure.

## SYNOPSIS
`#include <lightmodbus/slave.h>`

`void modbusSlaveEnd( ModbusSlaveStatus *status );`

## DESCRIPTION
The **modbusSlaveEnd** frees data dynamically allocated by **modbusSlaveInit**.

## SEE ALSO
ModbusSlaveStatus( 3lightmodbus ), modbusSlaveInit( 3lightmodbus )
