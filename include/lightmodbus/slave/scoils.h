#define _LIGHTMODBUS_SLAVE_COILS

#include <inttypes.h>

//Functions needed from other modules
extern uint8_t modbusBuildException( uint8_t, uint8_t );

//Functions for parsing requests
extern uint8_t modbusParseRequest01( union ModbusParser * );
extern uint8_t modbusParseRequest05( union ModbusParser * );
extern uint8_t modbusParseRequest15( union ModbusParser * );
