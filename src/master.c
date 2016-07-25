#include "../include/lightmodbus/master.h"

//Master configurations
MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusParseException( union MODBUSParser *Parser )
{
	//Parse exception frame and write data to MODBUSMaster structure

	//Check CRC
	if ( modbusCRC( ( *Parser ).Frame, 3 ) != ( *Parser ).Exception.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Copy data
	MODBUSMaster.Exception.Address = ( *Parser ).Exception.Address;
	MODBUSMaster.Exception.Function = ( *Parser ).Exception.Function;
	MODBUSMaster.Exception.Code = ( *Parser ).Exception.ExceptionCode;

	MODBUSMaster.Finished = 1;

	return MODBUS_ERROR_EXCEPTION;
}

uint8_t modbusParseResponse( uint8_t *Frame, uint8_t FrameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Note: CRC is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future CRC can be verified right here

	//If non-zero some parser failed its job
	uint8_t Error = 0;

	//Reset output registers before parsing frame
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Exception.Address = 0;
	MODBUSMaster.Exception.Function = 0;
	MODBUSMaster.Exception.Code = 0;
	MODBUSMaster.Finished = 0;

	//If user tries to parse an empty frame return error (to avoid problems with memory allocation)
	if ( FrameLength == 0 ) return MODBUS_ERROR_OTHER;

	//Allocate memory for union and copy frame to it
	union MODBUSParser *Parser = (union MODBUSParser *) malloc( FrameLength );
	if ( Parser == NULL )
	{
		free( Parser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( ( *Parser ).Frame,  Frame, FrameLength );

	//Allocate memory for request union and copy frame to it
	union MODBUSParser *RequestParser = (union MODBUSParser *) malloc( RequestFrameLength );
	if ( RequestParser == NULL )
	{
		free( Parser );
		free( RequestParser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( ( *RequestParser ).Frame,  RequestFrame, RequestFrameLength );

	//Check if frame is exception response
	if ( ( *Parser ).Base.Function & 128 )
	{
		Error = modbusParseException( Parser );
	}
	else
	{
		switch ( ( *Parser ).Base.Function )
		{
			case 1: //Read multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) Error = MODBUSParseResponse01( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 2: //Read multiple discrete inputs
				if ( LIGHTMODBUS_MASTER_DISCRETE_INPUTS ) Error = MODBUSParseResponse02( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 3: //Read multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = MODBUSParseResponse03( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 4: //Read multiple input registers
				if ( LIGHTMODBUS_MASTER_INPUT_REGISTERS ) Error = MODBUSParseResponse04( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 5: //Write single coil
				if ( LIGHTMODBUS_MASTER_COILS ) Error = MODBUSParseResponse05( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 6: //Write single holding register
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = MODBUSParseResponse06( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 15: //Write multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) Error = MODBUSParseResponse15( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			case 16: //Write multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) Error = MODBUSParseResponse16( Parser, RequestParser );
				else Error = MODBUS_ERROR_PARSE;
				break;

			default: //Function code not known by master
				Error = MODBUS_ERROR_PARSE;
				break;
		}
	}

	//Free used memory
	free( Parser );
	free( RequestParser );

	return Error;
}

uint8_t modbusMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.Request.Frame = (uint8_t *) malloc( 8 );
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Data = (MODBUSData_t *) malloc( sizeof( MODBUSData_t ) );
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 0;

	MODBUSMaster.Exception.Address = 0;
	MODBUSMaster.Exception.Function = 0;
	MODBUSMaster.Exception.Code = 0;

	return ( ( MODBUSMaster.Request.Frame == NULL ) || ( MODBUSMaster.Data == NULL ) ) * MODBUS_ERROR_ALLOC;
}

void MODBUSMasterDestroy( )
{
	//Free memory
	free( MODBUSMaster.Request.Frame );
	free( MODBUSMaster.Data );
}
