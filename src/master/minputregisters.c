#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/minputregisters.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusBuildRequest04( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount )
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *builder ).Base.Address = Address;
	( *builder ).Base.Function = 4;
	( *builder ).Request04.FirstRegister = modbusSwapEndian( FirstRegister );
	( *builder ).Request04.RegisterCount = modbusSwapEndian( RegisterCount );

	//Calculate CRC
	( *builder ).Request04.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse04( union MODBUSParser *parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 04
	//Read multiple input registers

	//Update frame length
	uint8_t FrameLength = 5 + ( *parser ).Response04.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Response04.Values[ ( *parser ).Response04.BytesCount >> 1 ] )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).Response04.Address == ( *RequestParser ).Request04.Address );
	DataOK &= ( ( *parser ).Response04.Function == ( *RequestParser ).Request04.Function );
	DataOK &= ( ( *parser ).Response04.BytesCount == modbusSwapEndian( ( *RequestParser ).Request04.RegisterCount ) << 1 );

	//If data is bad abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for MODBUSData_t structures array
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, ( ( *parser ).Response04.BytesCount >> 1 ) * sizeof( MODBUSData_t ) );

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *parser ).Response04.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = InputRegister;
		MODBUSMaster.Data[i].Register = modbusSwapEndian( ( *RequestParser ).Request04.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = modbusSwapEndian( ( *parser ).Response04.Values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = ( *parser ).Response04.BytesCount >> 1;
	MODBUSMaster.Finished = 1;
	return 0;
}
