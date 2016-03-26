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

uint8_t MODBUSParseResponse( uint8_t *Frame, uint8_t FrameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//If non-zero some parser failed its job
	uint8_t ParseSuccess = 0;

	//Allocate memory for union and copy frame to it
	union MODBUSParser *Parser = (union MODBUSParser *) malloc( FrameLength );
	memcpy( ( *Parser ).Frame,  Frame, FrameLength );

	//Allocate memory for request union and copy frame to it
	union MODBUSParser *RequestParser = (union MODBUSParser *) malloc( RequestFrameLength );
	memcpy( ( *RequestParser ).Frame,  RequestFrame, RequestFrameLength );

	//Reset output registers before parsing frame
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Error = 0;
	MODBUSMaster.Exception.Address = 0;
	MODBUSMaster.Exception.Function = 0;
	MODBUSMaster.Exception.Code = 0;

	//Check if frame is exception response
	if ( ( *Parser ).Base.Function & 128 )
	{
		MODBUSParseException( Parser );
	}
	else
	{
		switch ( MODBUS_MASTER_SUPPORT )
		{
			case 0: //Only base - no parsing
				break;

			case 1: //Basic support - basic parser
				ParseSuccess += MODBUSParseResponseBasic( Parser, RequestParser );
				break;
		}
	}

	//Free used memory
	free( Parser );
	free( RequestParser );

	if ( ParseSuccess > MODBUS_MASTER_SUPPORT - 1 ) return 1;
	else return 0;
}

void MODBUSMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.Request.Frame = (uint8_t *) malloc( 8 );
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Data = (MODBUSData *) malloc( sizeof( MODBUSData ) );
	MODBUSMaster.DataLength = 0;
}
