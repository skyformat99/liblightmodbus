#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/minputregisters.h"

//Use external master configuration
extern ModbusMasterStatus MODBUSMaster;

uint8_t modbusBuildRequest04( uint8_t address, uint16_t firstRegister, uint16_t registerCount )
{
	//Build request04 frame, to send it so slave
	//Read multiple input registers

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
	( *builder ).base.function = 4;
	( *builder ).request04.firstRegister = modbusSwapEndian( firstRegister );
	( *builder ).request04.registerCount = modbusSwapEndian( registerCount );

	//Calculate crc
	( *builder ).request04.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusParseResponse04( union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 04
	//Read multiple input registers

	//Update frame length
	uint8_t frameLength = 5 + ( *parser ).response04.byteCount;
	uint8_t dataok = 1;
	uint8_t i = 0;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).response04.values[ ( *parser ).response04.byteCount >> 1 ] )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( ( *parser ).response04.address == ( *requestParser ).request04.address );
	dataok &= ( ( *parser ).response04.function == ( *requestParser ).request04.function );
	dataok &= ( ( *parser ).response04.byteCount == modbusSwapEndian( ( *requestParser ).request04.registerCount ) << 1 );

	//If data is bad abort parsing, and set error flag
	if ( !dataok )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_FRAME;
	}

	//Allocate memory for ModbusData structures array
	MODBUSMaster.data = (ModbusData *) realloc( MODBUSMaster.data, ( ( *parser ).response04.byteCount >> 1 ) * sizeof( ModbusData ) );

	//Copy received data to output structures array
	for ( i = 0; i < ( ( *parser ).response04.byteCount >> 1 ); i++ )
	{
		MODBUSMaster.data[i].address = ( *parser ).base.address;
		MODBUSMaster.data[i].dataType = inputreg;
		MODBUSMaster.data[i].reg = modbusSwapEndian( ( *requestParser ).request04.firstRegister ) + i;
		MODBUSMaster.data[i].value = modbusSwapEndian( ( *parser ).response04.values[i] );
	}

	//Set up data length - response successfully parsed
	MODBUSMaster.dataLength = ( *parser ).response04.byteCount >> 1;
	MODBUSMaster.finished = 1;
	return 0;
}
