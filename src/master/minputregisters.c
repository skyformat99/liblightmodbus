#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/minputregisters.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t MODBUSBuildRequest04( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount )
{
	//Build request04 frame, to send it so slave
	//Read multiple input registers

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Finished = 0;

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 4;
	( *Builder ).Request04.FirstRegister = MODBUSSwapEndian( FirstRegister );
	( *Builder ).Request04.RegisterCount = MODBUSSwapEndian( RegisterCount );

	//Calculate CRC
	( *Builder ).Request04.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSParseResponse04( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
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
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response04.Address == ( *RequestParser ).Request04.Address );
	DataOK &= ( ( *Parser ).Response04.Function == ( *RequestParser ).Request04.Function );
	DataOK &= ( ( *Parser ).Response04.BytesCount == MODBUSSwapEndian( ( *RequestParser ).Request04.RegisterCount ) << 1 );

	//If data is bad abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for MODBUSData_t structures array
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, ( ( *Parser ).Response04.BytesCount >> 1 ) * sizeof( MODBUSData_t ) );

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *Parser ).Response04.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = InputRegister;
		MODBUSMaster.Data[i].Register = MODBUSSwapEndian( ( *RequestParser ).Request04.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = MODBUSSwapEndian( ( *Parser ).Response04.Values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = ( *Parser ).Response04.BytesCount >> 1;
	MODBUSMaster.Finished = 1;
	return 0;
}
