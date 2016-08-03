# modbusCRC 3lightmodbus "28 July 2016" "v1.2"

## NAME
**modbusCRC** - calculate Modbus CRC checksum of given data

## SYNOPSIS
`#include <lightmodbus/core.h>`

`uint16_t modbusCRC( uint8_t *data, uint16_t length );`

## DESCRIPTION
The **modbusCRC** function calculates and returns cyclic redundancy checksum of **length** bytes starting from **data** pointer.

## SEE ALSO
