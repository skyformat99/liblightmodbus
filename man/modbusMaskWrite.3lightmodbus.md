# modbusMaskWrite 3lightmodbus "28 July 2016" "v1.2"

## NAME
**modbusMaskWrite** - write single bit to an array.

## SYNOPSIS
`#include <lightmodbus/core.h>`

`uint8_t modbusMaskWrite( uint8_t *mask, uint16_t maskLength, uint16_t bit, uint8_t value );`

## DESCRIPTION
The **modbusMaskWrite** writes *bit* bit in *mask* little-endian array of *maskLength* length to *value* value. It's useful when whole array is
 one long bit mask. When returned value is greater than 0, an error occurred.

## SEE ALSO
