#include "modlib.h"
#include "parser.h"
#include "exception.h"

//Variables definitions
extern uint8_t MODBUSAddress; //Address of device
extern uint16_t *MODBUSRegisters; //Pointer to slave-side modbus registers
extern uint16_t MODBUSRegisterCount; //Count of slave-side modbus registers

//Function prototypes
extern void MODBUSParseRequest( uint8_t *, uint8_t ); //Parse and interpret given modbus frame on slave-side
