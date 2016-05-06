#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mregisters.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

uint8_t MODBUSBuildRequest03( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount )
{
	//Build request03 frame, to send it so slave
	//Read multiple holding registers

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
	( *Builder ).Base.Function = 3;
	( *Builder ).Request03.FirstRegister = MODBUSSwapEndian( FirstRegister );
	( *Builder ).Request03.RegisterCount = MODBUSSwapEndian( RegisterCount );

	//Calculate CRC
	( *Builder ).Request03.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSBuildRequest06( uint8_t Address, uint16_t Register, uint16_t Value )
{
	//Build request06 frame, to send it so slave
	//Write single holding register

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
	( *Builder ).Base.Function = 6;
	( *Builder ).Request06.Register = MODBUSSwapEndian( Register );
	( *Builder ).Request06.Value = MODBUSSwapEndian( Value );

	//Calculate CRC
	( *Builder ).Request06.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSBuildRequest16( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount, uint16_t *Values )
{
	//Build request16 frame, to send it so slave
	//Write multiple holding registers

	//Set frame length
	uint8_t FrameLength = 9 + ( RegisterCount << 1 );
	uint8_t i = 0;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Finished = 0;

	if ( RegisterCount > 123 ) return MODBUS_ERROR_OTHER;

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 16;
	( *Builder ).Request16.FirstRegister = MODBUSSwapEndian( FirstRegister );
	( *Builder ).Request16.RegisterCount = MODBUSSwapEndian( RegisterCount );
	( *Builder ).Request16.BytesCount = RegisterCount << 1;

	for ( i = 0; i < RegisterCount; i++ )
		( *Builder ).Request16.Values[i] = MODBUSSwapEndian( Values[i] );

	( *Builder ).Request16.Values[RegisterCount] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSParseResponse03( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 03
	//Read multiple holding registers

	//Update frame length
	uint8_t FrameLength = 5 + ( *Parser ).Response03.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response03.Values[ ( *Parser ).Response03.BytesCount >> 1 ] )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response03.Address == ( *RequestParser ).Request03.Address );
	DataOK &= ( ( *Parser ).Response03.Function == ( *RequestParser ).Request03.Function );
	DataOK &= ( ( *Parser ).Response03.BytesCount == MODBUSSwapEndian( ( *RequestParser ).Request03.RegisterCount ) << 1 );

	//If data is bad, abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for MODBUSData structures array
	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, ( ( *Parser ).Response03.BytesCount >> 1 ) * sizeof( MODBUSData ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *Parser ).Response03.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = Register;
		MODBUSMaster.Data[i].Register = MODBUSSwapEndian( ( *RequestParser ).Request03.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = MODBUSSwapEndian( ( *Parser ).Response03.Values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = ( *Parser ).Response03.BytesCount >> 1;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSParseResponse06( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 06 (write single holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response06.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response06.Address == ( *RequestParser ).Request06.Address );
	DataOK &= ( ( *Parser ).Response06.Function == ( *RequestParser ).Request06.Function );
	DataOK &= ( ( *Parser ).Response06.Register == ( *RequestParser ).Request06.Register );
	DataOK &= ( ( *Parser ).Response06.Value == ( *RequestParser ).Request06.Value );

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Swap endianness
	( *Parser ).Response06.Register = MODBUSSwapEndian( ( *Parser ).Response06.Register );
	( *Parser ).Response06.Value = MODBUSSwapEndian( ( *Parser ).Response06.Value );

	//Set up new data table
	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, sizeof( MODBUSData ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.Data[0].Address = ( *Parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Register;
	MODBUSMaster.Data[0].Register = ( *Parser ).Response06.Register;
	MODBUSMaster.Data[0].Value = ( *Parser ).Response06.Value;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
	return 0;
}

uint8_t MODBUSParseResponse16( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 16 (write multiple holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response16.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Response16.Address == ( *RequestParser ).Request16.Address );
	DataOK &= ( ( *Parser ).Response16.Function == ( *RequestParser ).Request16.Function );
	DataOK &= ( ( *Parser ).Response16.FirstRegister == ( *RequestParser ).Request16.FirstRegister );
	DataOK &= ( ( *Parser ).Response16.RegisterCount == ( *RequestParser ).Request16.RegisterCount );

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 1;
	return 0;
}
