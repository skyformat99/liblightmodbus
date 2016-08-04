# lightmodbus 3lightmodbus "4 August 2016" "v1.2"

## NAME
**lightmodbus** - a lightweight, multiplatform Modbus RTU library.

## DESCRIPTION
The **lightmodbus** library allows communication with use of Modbus RTU protocol. **lightmodbus** contains
functions for parsing and creating Modbus frames, but **it is not** sending or receiving them.
Modbus functions supported by library include: 01, 02, 03, 04, 05, 06, 15 and 16.
Library itself, is easy to compile and modular - only necessary modules can be included while building. Default version available for
PC is complete, and contains all modules. Needless to say, the library is possible to build at any little-endian platform.

## BUILDING
There are three makefiles attached to the library.
Usual `makefile` simply compiles source code, creating object files (`obj` directory), as well as static library files in `lib` directory.
Take a look at its content, to get a hint about how it works, and what you can change, during build process.
`makefile-coverage` compiles library for source code coverage testing (you probably don't need this, go on).

## BUILDING FOR AVR
`makefile-avr` is not automated, and each step should be called separately. To compile library for AVR more easily, use
`make-avr.py` script, that will guide you through build process.

## ROUTINES
Full listing of functions included in `lightmodbus` library.

| function name                 | manpage                  		 	            |
|-------------------------------|-----------------------------------------------|
| **modbusCRC**                 |  modbusCRC( 3lightmodbus )                    |
| **modbusSwapEndian**          |  modbusSwapEndian( 3lightmodbus )             |
| **modbusMaskRead**            |  modbusMaskRead( 3lightmodbus )               |
| **modbusMaskWrite**           |  modbusMaskWrite( 3lightmodbus )              |
| **modbusMasterInit**       	|  modbusMasterInit( 3lightmodbus )          	|
| **modbusMasterEnd**       	|  modbusMasterEnd( 3lightmodbus )          	|
| **modbusParseResponse**       |  modbusParseResponse( 3lightmodbus )          |
| **modbusParseException**      |  modbusParseException( 3lightmodbus )         |
| **modbusSlaveInit**      		|  modbusSlaveInit( 3lightmodbus )     		    |
| **modbusSlaveEnd**     		|  modbusSlaveEnd( 3lightmodbus )     		    |
| **modbusBuildException**      |  modbusBuildException( 3lightmodbus )         |
| **modbusParseRequest**   	   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusBuildRequest01**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest02**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest03**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest04**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest05**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest15**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusBuildRequest16**   	|  modbusBuildRequest( 3lightmodbus )         	|
| **modbusParseRequest01**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest02**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest03**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest04**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest05**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest15**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseRequest16**   	|  modbusParseRequest( 3lightmodbus )         	|
| **modbusParseResponse01**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse02**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse03**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse04**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse05**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse15**   	|  modbusParseResponse( 3lightmodbus )         	|
| **modbusParseResponse16**   	|  modbusParseResponse( 3lightmodbus )         	|

## USAGE

### Master
First of all, you need to make sure, that master module is included to the library. After that,
`lightmodbus/master.h` and all of its dependences header files are needed.

First step is to declare **ModbusMasterStatus** type variable, to contain Modbus configuration data. Later, it needs to be passed
to almost every master-side function you call.

`ModbusMasterStatus mstatus;`

Later, somewhere in your init function, remember to initiate master module with:

`modbusMasterInit( &mstatus );`

Then, you are free to do whatever you want. You can parse and build frames etc. Like this:

`modbusBuildRequest06( &mstatus, 32, 3, 1024 );`

Request frame will be stored in `mstatus.request.frame`. To know more, read modbusBuildRequest(3lightmodbus).

Parsing frames is done like this:

`modbusParseResponse( &mstatus );`

Beforehand, though, response frame from slave should be put into `mstatus.response`. Simple pointer assignment, and setting up length will do fine.
This assumes, that user did not change `mstatus.request`, because original request frame is needed to parse slave's response.


### Slave


## RETURN VALUES
**modbusSlaveInit**, **modbusSlaveEnd**, **modbusMasterInit**, **modbusMasterEnd**, **modbusParseRequest**, **modbusParseResponse**, **modbusBuildRequest** and the rest of parsing/building functions return an error code when they exit. Below you'll find description of those.

Error code macros are defined in **lightmodbus/core.h**.

`  
	#define MODBUS_ERROR_OK 0
	#define MODBUS_ERROR_PARSE 1
	#define MODBUS_ERROR_EXCEPTION 2
	#define MODBUS_ERROR_CRC 4
	#define MODBUS_ERROR_ALLOC 8
	#define MODBUS_ERROR_OTHER 16
	#define MODBUS_ERROR_FRAME 32
`

| macro                    | value | description                                |
|--------------------------|-------|--------------------------------------------|
| `MODBUS_ERROR_OK`        | 0     | no error occured, everything is fine       |
| `MODBUS_ERROR_PARSE`     | 1     | frame parsing error                        |
| `MODBUS_ERROR_EXCEPTION` | 2     | exception was eiher thrown or parsed       |
| `MODBUS_ERROR_CRC`       | 4     | frame CRC invalid (frame ignored)          |
| `MODBUS_ERROR_ALLOC`     | 8     | memory allocation error                    |
| `MODBUS_ERROR_OTHER`     | 16    | function was exited for other reason       |
| `MODBUS_ERROR_FRAME`     | 32    | frame contains incorrect data              |

`MODBUS_ERROR_OK` is returned when no error occurred.

`MODBUS_ERROR_PARSE` is returned when function code is not supported (library module missing?)

`MODBUS_ERROR_EXCEPTION` is returned either when:
	- (on slave side) slave had to return exception frame
	- (on master side) exception frame was parsed
	In both cases it's not an actual error, but an information for user.

`MODBUS_ERROR_CRC` is returned when CRC attached to frame was incorrect. Obviously frame is ignored in such a case.

`MODBUS_ERROR_ALLOC` is returned when **malloc** or **realloc** call has failed.

`MODBUS_ERROR_OTHER` is returned when e.g. user tries to parse frame of 0-length, or slave was initialized with address 0.

`MODBUS_ERROR_FRAME` is returned by master-side parsing function, when error was found in given frame (e.g. byte count doesn't match register count)
