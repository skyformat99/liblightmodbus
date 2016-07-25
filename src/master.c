#include "../include/lightmodbus/master.h"

//Master configurations
MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusParseException( union ModbusParser *parser )
{
	//Parse exception frame and write data to MODBUSMaster structure

	//Check crc
	if ( modbusCRC( ( *parser ).frame, 3 ) != ( *parser ).exception.crc )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Copy data
	MODBUSMaster.exception.address = ( *parser ).exception.address;
	MODBUSMaster.exception.function = ( *parser ).exception.function;
	MODBUSMaster.exception.Code = ( *parser ).exception.exceptionCode;

	MODBUSMaster.Finished = 1;

	return MODBUS_ERROR_EXCEPTION;
}

uint8_t modbusParseResponse( uint8_t *frame, uint8_t frameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Note: crc is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future crc can be verified right here

	//If non-zero some parser failed its job
	uint8_t Error = 0;

	//Reset output registers before parsing frame
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.exception.address = 0;
	MODBUSMaster.exception.function = 0;
	MODBUSMaster.exception.Code = 0;
	MODBUSMaster.Finished = 0;

	//If user tries to parse an empty frame return error (to avoid problems with memory allocation)
	if ( frameLength == 0 ) return MODBUS_ERROR_OTHER;

	//Allocate memory for union and copy frame to it
	union ModbusParser *parser = (union ModbusParser *) malloc( frameLength );
	if ( parser == NULL )
	{
		free( parser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( ( *parser ).frame,  frame, frameLength );

	//Allocate memory for request union and copy frame to it
	union ModbusParser *RequestParser = (union ModbusParser *) malloc( RequestFrameLength );
	if ( RequestParser == NULL )
	{
		free( parser );
		free( RequestParser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( ( *RequestParser ).frame,  RequestFrame, RequestFrameLength );

	//Check if frame is exception response
	if ( ( *parser ).base.function & 128 )
	{
		Error = modbusParseException( parser );
	}
	else
	{
		switch ( ( *parser ).base.function )
		{
			case 1: //Read multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) Error = modbusParseResponse01( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 2: //Read multiple discrete inputs
				if ( LIGHTMODBUS_MASTER_DISCRETE_INPUTS ) Error = modbusParseResponse02( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 3: //Read multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = modbusParseResponse03( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 4: //Read multiple input registers
				if ( LIGHTMODBUS_MASTER_INPUT_REGISTERS ) Error = modbusParseResponse04( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 5: //Write single coil
				if ( LIGHTMODBUS_MASTER_COILS ) Error = modbusParseResponse05( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 6: //Write single holding reg
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = modbusParseResponse06( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 15: //Write multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) Error = modbusParseResponse15( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 16: //Write multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = modbusParseResponse16( parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			default: //function code not known by master
				Error = MODBUS_ERROR_PARSE;
				break;
		}
	}

	//Free used memory
	free( parser );
	free( RequestParser );

	return Error;
}

uint8_t modbusMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.request.frame = (uint8_t *) malloc( 8 );
	MODBUSMaster.request.length = 0;
	MODBUSMaster.Data = (MODBUSData_t *) malloc( sizeof( MODBUSData_t ) );
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 0;

	MODBUSMaster.exception.address = 0;
	MODBUSMaster.exception.function = 0;
	MODBUSMaster.exception.Code = 0;

	return ( ( MODBUSMaster.request.frame == NULL ) || ( MODBUSMaster.Data == NULL ) ) * MODBUS_ERROR_ALLOC;
}

void modbusMasterEnd( )
{
	//Free memory
	free( MODBUSMaster.request.frame );
	free( MODBUSMaster.Data );
}
