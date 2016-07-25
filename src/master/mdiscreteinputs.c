#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mdiscreteinputs.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusBuildRequest02( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount )
{
	//Build request02 frame, to send it so slave
	//Read multiple discrete inputs

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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *builder ).Base.Address = Address;
	( *builder ).Base.Function = 2;
	( *builder ).Request02.FirstInput = modbusSwapEndian( FirstCoil );
	( *builder ).Request02.InputCount = modbusSwapEndian( CoilCount );

	//Calculate CRC
	( *builder ).Request02.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse02( union MODBUSParser *parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 02 (read multiple discrete inputs)

	//Update frame length
	uint8_t FrameLength = 5 + ( *parser ).Response02.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	DataOK &= ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) & 0x00FF ) == ( *parser ).Response02.Values[( *parser ).Response02.BytesCount];
	DataOK &= ( ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8 ) == ( *parser ).Response02.Values[( *parser ).Response02.BytesCount + 1];

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).Base.Address == ( *RequestParser ).Base.Address );
	DataOK &= ( ( *parser ).Base.Function == ( *RequestParser ).Base.Function );


	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, sizeof( MODBUSData_t ) * modbusSwapEndian( ( *RequestParser ).Request02.InputCount ) );
	for ( i = 0; i < modbusSwapEndian( ( *RequestParser ).Request02.InputCount ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = DiscreteInput;
		MODBUSMaster.Data[i].Register = modbusSwapEndian( ( *RequestParser ).Request02.FirstInput ) + i;
		MODBUSMaster.Data[i].Value = modbusMaskRead( ( *parser ).Response02.Values, ( *parser ).Response02.BytesCount, i );

	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = modbusSwapEndian( ( *RequestParser ).Request02.InputCount );
	MODBUSMaster.Finished = 1;

	return 0;
}
