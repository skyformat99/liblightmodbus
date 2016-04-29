#include "../modlib.h"
#include "../parser.h"
#include "mtypes.h"

#include "minputregisters.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

uint8_t MODBUSBuildRequest04( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount )
{
	//Build request04 frame, to send it so slave
	//Read multiple input registers

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 4;
	( *Builder ).Request04.FirstRegister = MODBUSSwapEndian( FirstRegister );
	( *Builder ).Request04.RegisterCount = MODBUSSwapEndian( RegisterCount );

	//Calculate CRC
	( *Builder ).Request04.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}

void MODBUSParseResponse04( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 04
	//Read multiple input registers

	//Update frame length
	uint8_t FrameLength = 5 + ( *Parser ).Response04.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response04.Values[ ( *Parser ).Response04.BytesCount >> 1 ] )
	{
		//Create an exception when CRC is bad (unoficially, but 255 is CRC internal exception code)
		MODBUSMaster.Exception.Address = ( *Parser ).Base.Address;
		MODBUSMaster.Exception.Function = ( *Parser ).Base.Function;
		MODBUSMaster.Exception.Code = 255;
		MODBUSMaster.Error = 1;
		MODBUSMaster.Finished = 1;
		return;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response04.Address == ( *RequestParser ).Request04.Address );
	DataOK &= ( ( *Parser ).Response04.Function == ( *RequestParser ).Request04.Function );
	DataOK &= ( ( *Parser ).Response04.BytesCount == MODBUSSwapEndian( ( *RequestParser ).Request04.RegisterCount ) << 1 );

	//If data is bad abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Error = 1;
		MODBUSMaster.Finished = 1;
		return;
	}

	//Allocate memory for MODBUSData structures array
	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, ( ( *Parser ).Response04.BytesCount >> 1 ) * sizeof( MODBUSData ) );

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *Parser ).Response04.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = InputRegister;
		MODBUSMaster.Data[i].Register = MODBUSSwapEndian( ( *RequestParser ).Request04.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = MODBUSSwapEndian( ( *Parser ).Response04.Values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.Error = !DataOK;
	MODBUSMaster.DataLength = ( *Parser ).Response04.BytesCount >> 1;
	MODBUSMaster.Finished = 1;
}
