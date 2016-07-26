#include "lightmodbus/core.h"
#include "lightmodbus/parser.h"
#include "lightmodbus/master/mtypes.h"
#include "lightmodbus/master/mcoils.h"

//Use external master configuration
extern ModbusMasterStatus MODBUSMaster;

uint8_t modbusBuildRequest01( uint8_t address, uint16_t firstCoil, uint16_t coilCount )
{
	//Build request01 frame, to send it so slave
	//Read multiple coils

	//Set frame length
	uint8_t frameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.finished = 0;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	( *builder ).base.address = address;
	( *builder ).base.function = 1;
	( *builder ).request01.firstCoil = modbusSwapEndian( firstCoil );
	( *builder ).request01.coilCount = modbusSwapEndian( coilCount );

	//Calculate crc
	( *builder ).request01.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusBuildRequest05( uint8_t address, uint16_t coil, uint16_t value )
{
	//Build request05 frame, to send it so slave
	//Write single coil

	//Set frame length
	uint8_t frameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.finished = 0;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	value = ( value != 0 ) ? 0xFF00 : 0x0000;

	( *builder ).base.address = address;
	( *builder ).base.function = 5;
	( *builder ).request05.coil = modbusSwapEndian( coil );
	( *builder ).request05.value = modbusSwapEndian( value );

	//Calculate crc
	( *builder ).request01.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusBuildRequest15( uint8_t address, uint16_t firstCoil, uint16_t coilCount, uint8_t *values )
{
	//Build request15 frame, to send it so slave
	//Write multiple coils

	//Set frame length
	uint8_t frameLength = 10 + ( ( coilCount - 1 ) >> 3 );
	uint8_t i = 0;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.request.length = 0;
	MODBUSMaster.finished = 0;

	if ( coilCount > 256 ) return 1;

	//Reallocate memory for final frame
	MODBUSMaster.request.frame = (uint8_t *) realloc( MODBUSMaster.request.frame, frameLength );
	if ( MODBUSMaster.request.frame == NULL )
	{
		free( MODBUSMaster.request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) MODBUSMaster.request.frame;

	( *builder ).base.address = address;
	( *builder ).base.function = 15;
	( *builder ).request15.firstCoil = modbusSwapEndian( firstCoil );
	( *builder ).request15.coilCount = modbusSwapEndian( coilCount );
	( *builder ).request15.byteCount = 1 + ( ( coilCount - 1 ) >> 3 );

	for ( i = 0; i < ( *builder ).request15.byteCount; i++ )
		( *builder ).request15.values[i] = values[i];

	( *builder ).frame[frameLength - 2] = modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0x00FF;
	( *builder ).frame[frameLength - 1] = ( modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0xFF00 ) >> 8;

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusParseResponse01( union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 01 (read multiple coils)

	//Update frame length
	uint8_t frameLength = 5 + ( *parser ).response01.byteCount;
	uint8_t dataok = 1;
	uint8_t i = 0;

	//Check frame crc
	dataok &= ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0x00FF ) == ( *parser ).response01.values[( *parser ).response01.byteCount];
	dataok &= ( ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0xFF00 ) >> 8 ) == ( *parser ).response01.values[( *parser ).response01.byteCount + 1];

	if ( !dataok )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( ( *parser ).base.address == ( *requestParser ).base.address );
	dataok &= ( ( *parser ).base.function == ( *requestParser ).base.function );


	MODBUSMaster.data = (ModbusData *) realloc( MODBUSMaster.data, sizeof( ModbusData ) * modbusSwapEndian( ( *requestParser ).request01.coilCount ) );
	if ( MODBUSMaster.data == NULL )
	{
		free( MODBUSMaster.data );
		return MODBUS_ERROR_ALLOC;
	}

	for ( i = 0; i < modbusSwapEndian( ( *requestParser ).request01.coilCount ); i++ )
	{
		MODBUSMaster.data[i].address = ( *parser ).base.address;
		MODBUSMaster.data[i].dataType = coil;
		MODBUSMaster.data[i].reg = modbusSwapEndian( ( *requestParser ).request01.firstCoil ) + i;
		MODBUSMaster.data[i].value = modbusMaskRead( ( *parser ).response01.values, ( *parser ).response01.byteCount, i );

	}

	//Set up data length - response successfully parsed
	MODBUSMaster.dataLength = modbusSwapEndian( ( *requestParser ).request01.coilCount );
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusParseResponse05( union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 05 (write single coil)

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t dataok = 1;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response05.crc )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( ( *parser ).base.address == ( *requestParser ).base.address );
	dataok &= ( ( *parser ).base.function == ( *requestParser ).base.function );


	MODBUSMaster.data = (ModbusData *) realloc( MODBUSMaster.data, sizeof( ModbusData ) );
	if ( MODBUSMaster.data == NULL )
	{
		free( MODBUSMaster.data );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSMaster.data[0].address = ( *parser ).base.address;
	MODBUSMaster.data[0].dataType = coil;
	MODBUSMaster.data[0].reg = modbusSwapEndian( ( *requestParser ).request05.coil );
	MODBUSMaster.data[0].value = ( *parser ).response05.value != 0;

	//Set up data length - response successfully parsed
	MODBUSMaster.dataLength = 1;
	MODBUSMaster.finished = 1;
	return 0;
}

uint8_t modbusParseResponse15( union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 15 (write multiple coils)

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t dataok = 1;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response15.crc )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( ( *parser ).base.address == ( *requestParser ).base.address );
	dataok &= ( ( *parser ).base.function == ( *requestParser ).base.function );
	dataok &= ( ( *parser ).response15.firstCoil == ( *requestParser ).request15.firstCoil );
	dataok &= ( ( *parser ).response15.coilCount == ( *requestParser ).request15.coilCount );

	//Set up data length - response successfully parsed
	MODBUSMaster.dataLength = 0;
	MODBUSMaster.finished = 1;
	return 0;
}
