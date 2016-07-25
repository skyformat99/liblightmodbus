#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mcoils.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusBuildRequest01( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount )
{
	//Build request01 frame, to send it so slave
	//Read multiple coils

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
	( *Builder ).Base.Function = 1;
	( *Builder ).Request01.FirstCoil = modbusSwapEndian( FirstCoil );
	( *Builder ).Request01.CoilCount = modbusSwapEndian( CoilCount );

	//Calculate CRC
	( *Builder ).Request01.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest05( uint8_t Address, uint16_t Coil, uint16_t Value )
{
	//Build request05 frame, to send it so slave
	//Write single coil

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

	Value = ( Value != 0 ) ? 0xFF00 : 0x0000;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 5;
	( *Builder ).Request05.Coil = modbusSwapEndian( Coil );
	( *Builder ).Request05.Value = modbusSwapEndian( Value );

	//Calculate CRC
	( *Builder ).Request01.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest15( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount, uint8_t *Values )
{
	//Build request15 frame, to send it so slave
	//Write multiple coils

	//Set frame length
	uint8_t FrameLength = 10 + ( ( CoilCount - 1 ) >> 3 );
	uint8_t i = 0;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Finished = 0;

	if ( CoilCount > 256 ) return 1;

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 15;
	( *Builder ).Request15.FirstCoil = modbusSwapEndian( FirstCoil );
	( *Builder ).Request15.CoilCount = modbusSwapEndian( CoilCount );
	( *Builder ).Request15.BytesCount = 1 + ( ( CoilCount - 1 ) >> 3 );

	for ( i = 0; i < ( *Builder ).Request15.BytesCount; i++ )
		( *Builder ).Request15.Values[i] = Values[i];

	( *Builder ).Frame[FrameLength - 2] = modbusCRC( ( *Builder ).Frame, FrameLength - 2 ) & 0x00FF;
	( *Builder ).Frame[FrameLength - 1] = ( modbusCRC( ( *Builder ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8;

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse01( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 01 (read multiple coils)

	//Update frame length
	uint8_t FrameLength = 5 + ( *Parser ).Response01.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	DataOK &= ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) & 0x00FF ) == ( *Parser ).Response01.Values[( *Parser ).Response01.BytesCount];
	DataOK &= ( ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8 ) == ( *Parser ).Response01.Values[( *Parser ).Response01.BytesCount + 1];

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *Parser ).Base.Function == ( *RequestParser ).Base.Function );


	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, sizeof( MODBUSData_t ) * modbusSwapEndian( ( *RequestParser ).Request01.CoilCount ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	for ( i = 0; i < modbusSwapEndian( ( *RequestParser ).Request01.CoilCount ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = Coil;
		MODBUSMaster.Data[i].Register = modbusSwapEndian( ( *RequestParser ).Request01.FirstCoil ) + i;
		MODBUSMaster.Data[i].Value = modbusMaskRead( ( *Parser ).Response01.Values, ( *Parser ).Response01.BytesCount, i );

	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = modbusSwapEndian( ( *RequestParser ).Request01.CoilCount );
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse05( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 05 (write single coil)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response05.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *Parser ).Base.Function == ( *RequestParser ).Base.Function );


	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, sizeof( MODBUSData_t ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.Data[0].Address = ( *Parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Coil;
	MODBUSMaster.Data[0].Register = modbusSwapEndian( ( *RequestParser ).Request05.Coil );
	MODBUSMaster.Data[0].Value = ( *Parser ).Response05.Value != 0;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
	return 0;
}

uint8_t modbusParseResponse15( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 15 (write multiple coils)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response15.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *Parser ).Base.Function == ( *RequestParser ).Base.Function );
	DataOK &= ( ( *Parser ).Response15.FirstCoil == ( *RequestParser ).Request15.FirstCoil );
	DataOK &= ( ( *Parser ).Response15.CoilCount == ( *RequestParser ).Request15.CoilCount );

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 1;
	return 0;
}
