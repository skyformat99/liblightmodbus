#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mregisters.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusBuildRequest03( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount )
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *builder ).Base.Address = Address;
	( *builder ).Base.Function = 3;
	( *builder ).Request03.FirstRegister = modbusSwapEndian( FirstRegister );
	( *builder ).Request03.RegisterCount = modbusSwapEndian( RegisterCount );

	//Calculate CRC
	( *builder ).Request03.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest06( uint8_t Address, uint16_t Register, uint16_t Value )
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *builder ).Base.Address = Address;
	( *builder ).Base.Function = 6;
	( *builder ).Request06.Register = modbusSwapEndian( Register );
	( *builder ).Request06.Value = modbusSwapEndian( Value );

	//Calculate CRC
	( *builder ).Request06.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest16( uint8_t Address, uint16_t FirstRegister, uint16_t RegisterCount, uint16_t *Values )
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSMaster.Request.Frame;

	( *builder ).Base.Address = Address;
	( *builder ).Base.Function = 16;
	( *builder ).Request16.FirstRegister = modbusSwapEndian( FirstRegister );
	( *builder ).Request16.RegisterCount = modbusSwapEndian( RegisterCount );
	( *builder ).Request16.BytesCount = RegisterCount << 1;

	for ( i = 0; i < RegisterCount; i++ )
		( *builder ).Request16.Values[i] = modbusSwapEndian( Values[i] );

	( *builder ).Request16.Values[RegisterCount] = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	MODBUSMaster.Request.Length = FrameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse03( union MODBUSParser *parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 03
	//Read multiple holding registers

	//Update frame length
	uint8_t FrameLength = 5 + ( *parser ).Response03.BytesCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Response03.Values[ ( *parser ).Response03.BytesCount >> 1 ] )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).Response03.Address == ( *RequestParser ).Request03.Address );
	DataOK &= ( ( *parser ).Response03.Function == ( *RequestParser ).Request03.Function );
	DataOK &= ( ( *parser ).Response03.BytesCount == modbusSwapEndian( ( *RequestParser ).Request03.RegisterCount ) << 1 );

	//If data is bad, abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for MODBUSData_t structures array
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, ( ( *parser ).Response03.BytesCount >> 1 ) * sizeof( MODBUSData_t ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *parser ).Response03.BytesCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].Address = ( *parser ).Base.Address;
		MODBUSMaster.Data[i].DataType = Register;
		MODBUSMaster.Data[i].Register = modbusSwapEndian( ( *RequestParser ).Request03.FirstRegister ) + i;
		MODBUSMaster.Data[i].Value = modbusSwapEndian( ( *parser ).Response03.Values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = ( *parser ).Response03.BytesCount >> 1;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse06( union MODBUSParser *parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 06 (write single holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Response06.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).Response06.Address == ( *RequestParser ).Request06.Address );
	DataOK &= ( ( *parser ).Response06.Function == ( *RequestParser ).Request06.Function );
	DataOK &= ( ( *parser ).Response06.Register == ( *RequestParser ).Request06.Register );
	DataOK &= ( ( *parser ).Response06.Value == ( *RequestParser ).Request06.Value );

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Swap endianness
	( *parser ).Response06.Register = modbusSwapEndian( ( *parser ).Response06.Register );
	( *parser ).Response06.Value = modbusSwapEndian( ( *parser ).Response06.Value );

	//Set up new data table
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, sizeof( MODBUSData_t ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.Data[0].Address = ( *parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Register;
	MODBUSMaster.Data[0].Register = ( *parser ).Response06.Register;
	MODBUSMaster.Data[0].Value = ( *parser ).Response06.Value;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
	return 0;
}

uint8_t modbusParseResponse16( union MODBUSParser *parser, union MODBUSParser *RequestParser )
{
	//Parse slave response to request 16 (write multiple holding register)

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t DataOK = 1;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Response16.CRC )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).Response16.Address == ( *RequestParser ).Request16.Address );
	DataOK &= ( ( *parser ).Response16.Function == ( *RequestParser ).Request16.Function );
	DataOK &= ( ( *parser ).Response16.FirstRegister == ( *RequestParser ).Request16.FirstRegister );
	DataOK &= ( ( *parser ).Response16.RegisterCount == ( *RequestParser ).Request16.RegisterCount );

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 1;
	return 0;
}
