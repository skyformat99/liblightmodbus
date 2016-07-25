#define _LIGHTMODBUS_MASTER_COILS

#include <inttypes.h>

//Functions for building requests
extern uint8_t modbusBuildRequest01( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest05( uint8_t, uint16_t, uint16_t );
extern uint8_t modbusBuildRequest15( uint8_t, uint16_t, uint16_t, uint8_t * );

//Functions for parsing responses
extern uint8_t modbusParseResponse01( union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse05( union ModbusParser *, union ModbusParser * );
extern uint8_t modbusParseResponse15( union ModbusParser *, union ModbusParser * );
