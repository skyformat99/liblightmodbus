#include "../include/modlib/master.h"

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

	//Note: CRC is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future CRC can be verified right here

	//If non-zero some parser failed its job
	uint8_t ParseError = 0;

	//Reset output registers before parsing frame
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Error = 0;
	MODBUSMaster.Exception.Address = 0;
	MODBUSMaster.Exception.Function = 0;
	MODBUSMaster.Exception.Code = 0;
	MODBUSMaster.Finished = 0;

	//If user tries to parse an empty frame return 2 (to avoid problems with memory allocation)
	if ( FrameLength == 0 ) return 2;

	//Allocate memory for union and copy frame to it
	union MODBUSParser *Parser = (union MODBUSParser *) malloc( FrameLength );
	memcpy( ( *Parser ).Frame,  Frame, FrameLength );

	//Allocate memory for request union and copy frame to it
	union MODBUSParser *RequestParser = (union MODBUSParser *) malloc( RequestFrameLength );
	memcpy( ( *RequestParser ).Frame,  RequestFrame, RequestFrameLength );

	//Check if frame is exception response
	if ( ( *Parser ).Base.Function & 128 )
	{
		MODBUSParseException( Parser );
	}
	else
	{
		switch ( ( *Parser ).Base.Function )
		{
			case 1: //Read multiple coils
				if ( MODBUS_MASTER_COILS ) MODBUSParseResponse01( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 2: //Read multiple discrete inputs
				if ( MODBUS_MASTER_DISCRETE_INPUTS ) MODBUSParseResponse02( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 3: //Read multiple holding registers
				if ( MODBUS_MASTER_REGISTERS ) MODBUSParseResponse03( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 4: //Read multiple input registers
				if ( MODBUS_MASTER_INPUT_REGISTERS ) MODBUSParseResponse04( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 5: //Write single coil
				if ( MODBUS_MASTER_COILS ) MODBUSParseResponse05( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 6: //Write single holding register
				if ( MODBUS_MASTER_REGISTERS ) MODBUSParseResponse06( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 15: //Write multiple coils
				if ( MODBUS_MASTER_COILS ) MODBUSParseResponse15( Parser, RequestParser );
				else ParseError = 1;
				break;

			case 16: //Write multiple holding registers
				if ( MODBUS_MASTER_REGISTERS ) MODBUSParseResponse16( Parser, RequestParser );
				else ParseError = 1;
				break;

			default: //Function code not known by master
				ParseError = 1;
				break;
		}
	}

	//Free used memory
	free( Parser );
	free( RequestParser );

	return ParseError;
}

void MODBUSMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.Request.Frame = (uint8_t *) malloc( 8 );
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Data = (MODBUSData *) malloc( sizeof( MODBUSData ) );
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 0;

	MODBUSMaster.Error = 0;
	MODBUSMaster.Exception.Address = 0;
	MODBUSMaster.Exception.Function = 0;
	MODBUSMaster.Exception.Code = 0;
}
