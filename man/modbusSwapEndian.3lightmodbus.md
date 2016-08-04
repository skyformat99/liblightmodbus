# modbusSwapEndian 3lightmodbus "28 July 2016" "v1.2"

## NAME
**modbusSwapEndian** - swap given 16-bit integer's endianness.

## SYNOPSIS
`#include <lightmodbus/core.h>`

`uint16_t modbusSwapEndian( uint16_t data );`

## DESCRIPTION
The **modbusSwapEndian** function returns same 16-bit portion of data, but with bytes order swapped. Function is included, because most PCs
are little-endian, while Modbus protocol uses big-endian data format.   

## SEE ALSO

## AUTHORS
Jacek Wieczorek (Jacajack) <mrjjot@gmail.com>
