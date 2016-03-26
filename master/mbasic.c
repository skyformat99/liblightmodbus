#include "../modlib.h"
#include "../parser.h"
#include "mtypes.h"

#include "mbasic.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

void MODBUSBuildRequest06( uint8_t Address, uint16_t Register, uint16_t Value )
{
	//Build request06 frame, to send it so slave

	//Set frame length
	uint8_t FrameLength = 8;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 6;
	( *Builder ).Request06.Register = MODBUSSwapEndian( Register );
	( *Builder ).Request06.Value = MODBUSSwapEndian( Value );

	//Calculate CRC
	( *Builder ).Request06.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;
}

void MODBUSParseResponse03( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 03 (read multiple holding registers)

	//Update frame length
	uint8_t FrameLength = 5 + ( *Parser ).Response03.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response03.Values[ ( *Parser ).Response03.BytesCount >> 1 ] ) return;

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response03.Address == ( *RequestParser ).Request03.Address );
	DataOK &= ( ( *Parser ).Response03.Function == ( *RequestParser ).Request03.Function );
	DataOK &= ( ( *Parser ).Response03.BytesCount == MODBUSSwapEndian( ( *RequestParser ).Request03.RegisterCount ) << 1 );

	if ( !DataOK )
	{
		MODBUSMaster.Error = 1;
		MODBUSMaster.Finished = 1;
		return;
	}

	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, ( ( *Parser ).Response03.BytesCount >> 1 ) * sizeof( MODBUSData ) );

	for ( i = 0; i < ( ( *Parser ).Response03.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = Register;
		MODBUSMaster.Data[i].Register = MODBUSSwapEndian( ( *RequestParser ).Request03.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = MODBUSSwapEndian( ( *Parser ).Response03.Values[i] );
	}

	MODBUSMaster.DataLength = ( *Parser ).Response03.BytesCount >> 1;
	MODBUSMaster.Finished = 1;
}

void MODBUSParseResponse06( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 06 (write single holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response06.CRC ) return;

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response06.Address == ( *RequestParser ).Request06.Address );
	DataOK &= ( ( *Parser ).Response06.Function == ( *RequestParser ).Request06.Function );
	DataOK &= ( ( *Parser ).Response06.Register == ( *RequestParser ).Request06.Register );
	DataOK &= ( ( *Parser ).Response06.Value == ( *RequestParser ).Request06.Value );

	if ( !DataOK )
	{
		MODBUSMaster.Error = 1;
		MODBUSMaster.Finished = 1;
		return;
	}

	//Swap endianness
	( *Parser ).Response06.Register = MODBUSSwapEndian( ( *Parser ).Response06.Register );
	( *Parser ).Response06.Value = MODBUSSwapEndian( ( *Parser ).Response06.Value );

	//Set up new data table
	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, sizeof( MODBUSData ) );
	MODBUSMaster.Data[0].Address = ( *Parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Register;
	MODBUSMaster.Data[0].Register = ( *Parser ).Response06.Register;
	MODBUSMaster.Data[0].Value = ( *Parser ).Response06.Value;

	MODBUSMaster.Error = 0;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
}

void MODBUSParseResponse16( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 16 (write multiple holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response16.CRC ) return;

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response16.Address == ( *RequestParser ).Request16.Address );
	DataOK &= ( ( *Parser ).Response16.Function == ( *RequestParser ).Request16.Function );
	DataOK &= ( ( *Parser ).Response16.FirstRegister == ( *RequestParser ).Request16.FirstRegister );
	DataOK &= ( ( *Parser ).Response16.RegisterCount == ( *RequestParser ).Request16.RegisterCount );

	MODBUSMaster.Error = !DataOK;
	MODBUSMaster.Finished = 1;
}

uint8_t MODBUSParseResponseBasic( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse response frame returned by slave using basic parsing functions
	//If 0 is returned everything is ok, and response is parsed
	//Otherwise function code is not supported and you should try full parser instead
	//Or maybe function code is wrong itself

	switch( ( *Parser ).Base.Function )
	{
		case 3: //Read multiple holding registers
			MODBUSParseResponse03( Parser, RequestParser );
			return 0;
			break;

		case 6: //Write single holding register
			MODBUSParseResponse06( Parser, RequestParser );
			return 0;
			break;

		case 16: //Write multiple holding registers
			MODBUSParseResponse16( Parser, RequestParser );
			return 0;
			break;

		default:
			return 1;
			break;
	}
}
