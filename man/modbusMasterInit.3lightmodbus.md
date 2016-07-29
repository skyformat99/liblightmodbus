# modbusMasterInit 3lightmodbus "29 July 2016" "v1.2"

## NAME
**modbusMasterInit** - setup **ModbusMasterStatus** structure for use.

## SYNOPSIS
`#include <lightmodbus/master.h>`

`uint8_t modbusMasterInit( ModbusMasterStatus *status );`

## DESCRIPTION
The **modbusMasterInit** function sets crucial members of **ModbusMasterStatus** structure and allocates memory for necessary data. This function should always be called before using any other Modbus master-side functions.
Needless to say, when returned value is not equal 0 an error occured.

Memory can be later freed with **modbusMasterEnd**.

## SEE ALSO
ModbusMasterStatus( 3lightmodbus ), modbusMasterEnd( 3lightmodbus )
