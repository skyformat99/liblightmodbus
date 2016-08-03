# modbusMaskRead 3lightmodbus "28 July 2016" "v1.2"

## NAME
**modbusMaskRead** - read single bit from an array.

## SYNOPSIS
`#include <lightmodbus/core.h>`

`uint8_t modbusMaskRead( uint8_t *mask, uint16_t maskLength, uint16_t bit );`

## DESCRIPTION
The **modbusMaskRead** function returns **bit** bit from **mask** little-endian array of **maskLength** length. It's useful when whole array is
 one long bit mask. When returned value is greater than 1, an error occurred.

## SEE ALSO
