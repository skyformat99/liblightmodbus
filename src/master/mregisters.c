#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mregisters.h"

//Use external master configuration
extern MODBUSMasterStatus_t MODBUSMaster;

uint8_t modbusBuildRequest03( uint8_t address, uint16_t firstRegister, uint16_t registerCount )
{
	//Build request03 frame, to send it so slave
	//Read multiple holding registers

	//Set frame length
	uint8_t frameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.Finished = 0;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	( *builder ).base.address = address;
	( *builder ).base.function = 3;
	( *builder ).request03.firstRegister = modbusSwapEndian( firstRegister );
	( *builder ).request03.registerCount = modbusSwapEndian( registerCount );

	//Calculate crc
	( *builder ).request03.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest06( uint8_t address, uint16_t reg, uint16_t value )
{
	//Build request06 frame, to send it so slave
	//Write single holding reg

	//Set frame length
	uint8_t frameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.Finished = 0;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	( *builder ).base.address = address;
	( *builder ).base.function = 6;
	( *builder ).request06.reg = modbusSwapEndian( reg );
	( *builder ).request06.value = modbusSwapEndian( value );

	//Calculate crc
	( *builder ).request06.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusBuildRequest16( uint8_t address, uint16_t firstRegister, uint16_t registerCount, uint16_t *values )
{
	//Build request16 frame, to send it so slave
	//Write multiple holding registers

	//Set frame length
	uint8_t frameLength = 9 + ( registerCount << 1 );
	uint8_t i = 0;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.Finished = 0;

	if ( registerCount > 123 ) return MODBUS_ERROR_OTHER;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	( *builder ).base.address = address;
	( *builder ).base.function = 16;
	( *builder ).request16.firstRegister = modbusSwapEndian( firstRegister );
	( *builder ).request16.registerCount = modbusSwapEndian( registerCount );
	( *builder ).request16.byteCount = registerCount << 1;

	for ( i = 0; i < registerCount; i++ )
		( *builder ).request16.values[i] = modbusSwapEndian( values[i] );

	( *builder ).request16.values[registerCount] = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse03( union ModbusParser *parser, union ModbusParser *RequestParser )
{
	//Parse slave response to request 03
	//Read multiple holding registers

	//Update frame length
	uint8_t frameLength = 5 + ( *parser ).response03.byteCount;
	uint8_t DataOK = 1;
	uint8_t i = 0;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response03.values[ ( *parser ).response03.byteCount >> 1 ] )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).response03.address == ( *RequestParser ).request03.address );
	DataOK &= ( ( *parser ).response03.function == ( *RequestParser ).request03.function );
	DataOK &= ( ( *parser ).response03.byteCount == modbusSwapEndian( ( *RequestParser ).request03.registerCount ) << 1 );

	//If data is bad, abort parsing, and set error flag
	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for MODBUSData_t structures array
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, ( ( *parser ).response03.byteCount >> 1 ) * sizeof( MODBUSData_t ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *parser ).response03.byteCount >> 1 ); i++ )
	{
		MODBUSMaster.Data[i].address = ( *parser ).base.address;
		MODBUSMaster.Data[i].DataType = reg;
		MODBUSMaster.Data[i].reg = modbusSwapEndian( ( *RequestParser ).request03.firstRegister ) + i;
		MODBUSMaster.Data[i].value = modbusSwapEndian( ( *parser ).response03.values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = ( *parser ).response03.byteCount >> 1;
	MODBUSMaster.Finished = 1;

	return 0;
}

uint8_t modbusParseResponse06( union ModbusParser *parser, union ModbusParser *RequestParser )
{
	//Parse slave response to request 06 (write single holding reg)

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t DataOK = 1;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response06.crc )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).response06.address == ( *RequestParser ).request06.address );
	DataOK &= ( ( *parser ).response06.function == ( *RequestParser ).request06.function );
	DataOK &= ( ( *parser ).response06.reg == ( *RequestParser ).request06.reg );
	DataOK &= ( ( *parser ).response06.value == ( *RequestParser ).request06.value );

	if ( !DataOK )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Swap endianness
	( *parser ).response06.reg = modbusSwapEndian( ( *parser ).response06.reg );
	( *parser ).response06.value = modbusSwapEndian( ( *parser ).response06.value );

	//Set up new data table
	MODBUSMaster.Data = (MODBUSData_t *) realloc( MODBUSMaster.Data, sizeof( MODBUSData_t ) );
	if ( MODBUSMaster.Data == NULL )
	{
		free( MODBUSMaster.Data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.Data[0].address = ( *parser ).base.address;
	MODBUSMaster.Data[0].DataType = reg;
	MODBUSMaster.Data[0].reg = ( *parser ).response06.reg;
	MODBUSMaster.Data[0].value = ( *parser ).response06.value;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
	MODBUSMaster.Finished = 1;
	return 0;
}

uint8_t modbusParseResponse16( union ModbusParser *parser, union ModbusParser *RequestParser )
{
	//Parse slave response to request 16 (write multiple holding reg)

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t DataOK = 1;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response16.crc )
	{
		MODBUSMaster.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	DataOK &= ( ( *parser ).response16.address == ( *RequestParser ).request16.address );
	DataOK &= ( ( *parser ).response16.function == ( *RequestParser ).request16.function );
	DataOK &= ( ( *parser ).response16.firstRegister == ( *RequestParser ).request16.firstRegister );
	DataOK &= ( ( *parser ).response16.registerCount == ( *RequestParser ).request16.registerCount );

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 0;
	MODBUSMaster.Finished = 1;
	return 0;
}
