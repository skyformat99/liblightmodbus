# lightmodbus 3lightmodbus "30 July 2016" "v1.2"

## NAME
**lightmodbus** - a lightweight, multiplatform Modbus RTU library.

## ERRORS
`  
	#define MODBUS_ERROR_OK 0
	#define MODBUS_ERROR_PARSE 1
	#define MODBUS_ERROR_EXCEPTION 2
	#define MODBUS_ERROR_CRC 4
	#define MODBUS_ERROR_ALLOC 8
	#define MODBUS_ERROR_OTHER 16
	#define MODBUS_ERROR_FRAME 32
`

Error code macros are defined in **lightmodbus/core.h**.

| macro                    | value | description                              |
|--------------------------|-------|------------------------------------------|
| `MODBUS_ERROR_OK`        | 0     | no error occured, everything is fine     |
| `MODBUS_ERROR_PARSE`     | 1     | frame parsing error                      |
| `MODBUS_ERROR_EXCEPTION` | 2     | exception was eiher thrown or parsed     |
| `MODBUS_ERROR_CRC`       | 4     | frame CRC invalid (frame ignored)        |
| `MODBUS_ERROR_ALLOC`     | 8     | memory allocation error                  |
| `MODBUS_ERROR_OTHER`     | 16    | function was exited for other reason     |
| `MODBUS_ERROR_FRAME`     | 32    | frame contains incorrect data            |


`MODBUS_ERROR_PARSE` is usually thrown when function code is not supported (library module missing?)

`MODBUS_ERROR_EXCEPTION` is returned either when:
	- (on slave side) slave had to return exception frame
	- (on master side) exception frame was parsed
	In both cases it's not an actual error, but an information for user.

`MODBUS_ERROR_ALLOC` is returned when **malloc** or **realloc** call has failed.

`MODBUS_ERROR_OTHER` is thrown when e.g. user tries to parse frame of 0-length, or slave was initialized with address 0.

`MODBUS_ERROR_FRAME` can be returned by master-side parsing function, when error was found in given frame (e.g. byte count doesn't match register count)
