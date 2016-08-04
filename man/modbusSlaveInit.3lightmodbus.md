# modbusSlaveInit 3lightmodbus "29 July 2016" "v1.2"

## NAME
**modbusSlaveInit** - setup **ModbusSlaveStatus** structure for use.

## SYNOPSIS
`#include <lightmodbus/slave.h>`

`uint8_t modbusSlaveInit( ModbusSlaveStatus *status );`

## DESCRIPTION
The **modbusSlaveInit** function sets crucial members of **ModbuSlaveStatus** structure and allocates memory for necessary data. This function should always be called before using any other Modbus slave-side functions.
It is also worth mentioning, that memory for *status.request* is **not** allocated (user should perform simple pointer assignment, not data copying).
Needless to say, when returned value is not equal 0 an error occured.

Memory can be later freed with **modbusSlaveEnd**.

## SEE ALSO
ModbusSlaveStatus(3lightmodbus), modbusSlaveEnd(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
