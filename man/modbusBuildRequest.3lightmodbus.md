# modbusBuildRequest 3lightmodbus "4 August 2016" "v1.2"

## NAME
**modbusBuildRequest**, **modbusBuildRequest01**, **modbusBuildRequest02**, **modbusBuildRequest03**, **modbusBuildRequest04**, **modbusBuildRequest05**, **modbusBuildRequest06**, **modbusBuildRequest15**, **modbusBuildRequest16** - build request for slave device.

## SYNOPSIS
`#include <lightmodbus/master.h>`

`  
	uint8_t modbusBuildRequest01( ModbusMasterStatus *status, uint8_t address, uint16_t firstCoil, uint16_t coilCount );
	uint8_t modbusBuildRequest02( ModbusMasterStatus *status, uint8_t address, uint16_t firstInput, uint16_t inputCount );
	uint8_t modbusBuildRequest03( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount );
	uint8_t modbusBuildRequest04( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount );
	uint8_t modbusBuildRequest05( ModbusMasterStatus *status, uint8_t address, uint16_t coil, uint16_t value );
	uint8_t modbusBuildRequest06( ModbusMasterStatus *status, uint8_t address, uint16_t reg, uint16_t value );
	uint8_t modbusBuildRequest15( ModbusMasterStatus *status, uint8_t address, uint16_t firstCoil, uint16_t coilCount, uint8_t *values );
	uint8_t modbusBuildRequest16( ModbusMasterStatus *status, uint8_t address, uint16_t firstRegister, uint16_t registerCount, uint16_t *values );
`

## DESCRIPTION
The **modbusBuildRequest** functions build request frame later located in *status.request*, ought to be sent to slave device.
Function prototypes are rather self-explanatory.
An error code is returned (described in lightmodbus(3lightmodbus)).

## SEE ALSO
lightmodbus(3lightmodbus), ModbusMasterStatus(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
