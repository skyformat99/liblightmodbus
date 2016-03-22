#include "master.h"

//Master configurations
MODBUSMasterStatus MODBUSMaster;

void MODBUSParseException( union MODBUSParser *Parser )
{
	//Parse exception frame and write data to MODBUSMaster structure

	//Check CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, 3 ) != ( *Parser ).Exception.CRC ) return;

	//Copy data
	MODBUSMaster.Exception.Address = ( *Parser ).Exception.Address;
	MODBUSMaster.Exception.Function = ( *Parser ).Exception.Function;
	MODBUSMaster.Exception.Code = ( *Parser ).Exception.ExceptionCode;

	MODBUSMaster.Error = 1;
}

void MODBUSParseResponse( uint8_t *Frame, uint8_t FrameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Allocate memory for union and copy frame to it
	union MODBUSParser *Parser = malloc( FrameLength );
	memcpy( ( *Parser ).Frame,  Frame, FrameLength );

	//Allocate memory for request union and copy frame to it
	union MODBUSParser *RequestParser = malloc( RequestFrameLength );
	memcpy( ( *RequestParser ).Frame,  RequestFrame, RequestFrameLength );

	//Check if frame is exception response
	if ( ( *Parser ).Base.Function & 128 )
	{
		MODBUSParseException( Parser );
	}
	else
	{
		if ( MODBUS_MASTER_BASIC )
			MODBUSParseResponseBasic( Parser, RequestParser );
	}

	//Free used memory
	free( Parser );
	free( RequestParser );
}

void MODBUSMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.Request.Frame = malloc( 8 );
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Data = malloc( sizeof( MODBUSData ) );
	MODBUSMaster.DataLength = 0;
}
