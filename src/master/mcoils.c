#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/master/mtypes.h"
#include "../../include/modlib/master/mcoils.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

uint8_t MODBUSBuildRequest01( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount )
{
	//Build request01 frame, to send it so slave
	//Read multiple coils

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );
	if ( Builder == NULL )
	{
		free( Builder );
		return MODBUS_ERROR_ALLOC;
	}

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( Builder );
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 1;
	( *Builder ).Request01.FirstCoil = MODBUSSwapEndian( FirstCoil );
	( *Builder ).Request01.CoilCount = MODBUSSwapEndian( CoilCount );

	//Calculate CRC
	( *Builder ).Request01.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}

uint8_t MODBUSBuildRequest05( uint8_t Address, uint16_t Coil, uint16_t Value )
{
	//Build request05 frame, to send it so slave
	//Write single coil

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );
	if ( Builder == NULL )
	{
		free( Builder );
		return MODBUS_ERROR_ALLOC;
	}

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( Builder );
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}

	Value = ( Value != 0 ) ? 0xFF00 : 0x0000;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 5;
	( *Builder ).Request05.Coil = MODBUSSwapEndian( Coil );
	( *Builder ).Request05.Value = MODBUSSwapEndian( Value );

	//Calculate CRC
	( *Builder ).Request01.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}

uint8_t MODBUSBuildRequest15( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount, uint8_t *Values )
{
	//Build request15 frame, to send it so slave
	//Write multiple coils

	//Set frame length
	uint8_t FrameLength = 10 + ( ( CoilCount - 1 ) >> 3 );
	uint8_t i = 0;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	if ( CoilCount > 256 ) return 1;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );
	if ( Builder == NULL )
	{
		free( Builder );
		return MODBUS_ERROR_ALLOC;
	}

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );
	if ( MODBUSMaster.Request.Frame == NULL )
	{
		free( Builder );
		free( MODBUSMaster.Request.Frame );
		return MODBUS_ERROR_ALLOC;
	}

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 15;
	( *Builder ).Request15.FirstCoil = MODBUSSwapEndian( FirstCoil );
	( *Builder ).Request15.CoilCount = MODBUSSwapEndian( CoilCount );
	( *Builder ).Request15.BytesCount = 1 + ( ( CoilCount - 1 ) >> 3 );

	for ( i = 0; i < ( *Builder ).Request15.BytesCount; i++ )
		( *Builder ).Request15.Values[i] = Values[i];

	( *Builder ).Frame[FrameLength - 2] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 ) & 0x00FF;
	( *Builder ).Frame[FrameLength - 1] = ( MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8;

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}

uint8_t MODBUSParseResponse01( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 01 (read multiple coils)

	//Update frame length
	uint8_t FrameLength = 5 + ( *Parser ).Response01.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	DataOK &= ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) & 0x00FF ) == ( *Parser ).Response01.Values[( *Parser ).Response01.BytesCount];
	DataOK &= ( ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8 ) == ( *Parser ).Response01.Values[( *Parser ).Response01.BytesCount + 1];

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *Parser ).Base.Function == ( *RequestParser ).Base.Function );


	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, sizeof( MODBUSData ) * MODBUSSwapEndian( ( *RequestParser ).Request01.CoilCount ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	for ( i = 0; i < MODBUSSwapEndian( ( *RequestParser ).Request01.CoilCount ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *Parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = Coil;
		MODBUSMaster.Data[i].Register = MODBUSSwapEndian( ( *RequestParser ).Request01.FirstCoil ) + i;
		MODBUSMaster.Data[i].Value = MODBUSReadMaskBit( ( *Parser ).Response01.Values, ( *Parser ).Response01.BytesCount, i );

	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = MODBUSSwapEndian( ( *RequestParser ).Request01.CoilCount );
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t MODBUSParseResponse05( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 05 (write single coil)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response05.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *Parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *Parser ).Base.Function == ( *RequestParser ).Base.Function );


	MODBUSMaster.Data = (MODBUSData *) realloc( MODBUSMaster.Data, sizeof( MODBUSData ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.Data[0].Address = ( *Parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Coil;
	MODBUSMaster.Data[0].Register = MODBUSSwapEndian( ( *RequestParser ).Request05.Coil );
	MODBUSMaster.Data[0].Value = ( *Parser ).Response05.Value != 0;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
	return 0;
}

uint8_t MODBUSParseResponse15( union MODBUSParser *Parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 15 (write multiple coils)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response15.CRC )
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
