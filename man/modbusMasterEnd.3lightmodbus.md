# modbusMasterEnd 3lightmodbus "29 July 2016" "v1.2"

## NAME
**modbusMasterEnd** - free memory used in **ModbusMasterStatus** structure.

## SYNOPSIS
`#include <lightmodbus/master.h>`

`void modbusMasterEnd( ModbusMasterStatus *status );`

## DESCRIPTION
The **modbusMasterEnd** frees data dynamically allocated by **modbusMasterInit**.

## SEE ALSO
ModbusMasterStatus(3lightmodbus), modbusMasterInit(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
