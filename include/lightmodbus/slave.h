#ifndef LIGHTMODBUS_SLAVE_BASE_H
#define LIGHTMODBUS_SLAVE_BASE_H

#include <inttypes.h>

#include "lightmodbus/slave/stypes.h"

//Enabling modules in compilation process (use makefile to automate this process)
#ifndef LIGHTMODBUS_SLAVE_REGISTERS
#define LIGHTMODBUS_SLAVE_REGISTERS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_COILS
#define LIGHTMODBUS_SLAVE_COILS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_DISCRETE_INPUTS
#define LIGHTMODBUS_SLAVE_DISCRETE_INPUTS 0
#endif
#ifndef LIGHTMODBUS_SLAVE_INPUT_REGISTERS
#define LIGHTMODBUS_SLAVE_INPUT_REGISTERS 0
#endif

//function prototypes
extern uint8_t modbusParseRequest( ModbusSlaveStatus *status, uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
extern uint8_t modbusSlaveInit( ModbusSlaveStatus *status, uint8_t ); //Very basic init of slave side
extern void modbusSlaveEnd( ModbusSlaveStatus *status ); //Free memory used by slave

#endif
