# lightmodbus 3lightmodbus "30 July 2016" "v1.2"

## NAME
**lightmodbus** - a lightweight, multiplatform Modbus RTU library.

## ERRORS
`#define MODBUS_ERROR_OK 0
#define MODBUS_ERROR_PARSE 1
#define MODBUS_ERROR_EXCEPTION 2
#define MODBUS_ERROR_CRC 4
#define MODBUS_ERROR_ALLOC 8
#define MODBUS_ERROR_OTHER 16
#define MODBUS_ERROR_FRAME 32`

Error code macros are defined in **lightmodbus/core.h**.

| macro                      | value | description                                                                                                  |
|----------------------------|-------|--------------------------------------------------------------------------------------------------------------|
| **MODBUS_ERROR_OK**        | 0     | no error occured, everything is fine                                                                         |
| **MODBUS_ERROR_PARSE**     | 1     | frame parsing error occured (function code not supported)                                                    |
| **MODBUS_ERROR_EXCEPTION** | 2     | frame parsed successfully, but slave threw an exception for some reason                                      |
| **MODBUS_ERROR_CRC**       | 4     | frame CRC invalid (frame ignored)                                                                            |
| **MODBUS_ERROR_ALLOC**     | 8     | memory allocation error                                                                                      |
| **MODBUS_ERROR_OTHER**     | 16    | function was exited for another reason (e.g. tried to parse frame of 0 length, slave set up with address 0)  |
| **MODBUS_ERROR_FRAME**     | 32    | frame contains incorrect data (e.g byte count does not match register count)                                 |
