#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/master/mtypes.h"
#include "../../include/lightmodbus/master/mdiscreteinputs.h"

//Use external master configuration
extern ModbusMasterStatus MODBUSMaster;

uint8_t modbusBuildRequest02( uint8_t address, uint16_t firstCoil, uint16_t coilCount )
{
	//Build request02 frame, to send it so slave
	//Read multiple discrete inputs

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
	( *builder ).base.function = 2;
	( *builder ).request02.firstInput = modbusSwapEndian( firstCoil );
	( *builder ).request02.inputCount = modbusSwapEndian( coilCount );

	//Calculate crc
	( *builder ).request02.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	MODBUSMaster.request.length = frameLength;
	MODBUSMaster.finished = 1;

	return 0;
}

uint8_t modbusParseResponse02( union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 02 (read multiple discrete inputs)

	//Update frame length
	uint8_t frameLength = 5 + ( *parser ).response02.byteCount;
	uint8_t dataok = 1;
	uint8_t i = 0;

	//Check frame crc
	dataok &= ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0x00FF ) == ( *parser ).response02.values[( *parser ).response02.byteCount];
	dataok &= ( ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0xFF00 ) >> 8 ) == ( *parser ).response02.values[( *parser ).response02.byteCount + 1];

	if ( !dataok )
	{
		MODBUSMaster.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( ( *parser ).base.address == ( *requestParser ).base.address );
	dataok &= ( ( *parser ).base.function == ( *requestParser ).base.function );


	MODBUSMaster.data = (ModbusData *) realloc( MODBUSMaster.data, sizeof( ModbusData ) * modbusSwapEndian( ( *requestParser ).request02.inputCount ) );
	for ( i = 0; i < modbusSwapEndian( ( *requestParser ).request02.inputCount ); i++ )
	{
		MODBUSMaster.data[i].address = ( *parser ).base.address;
		MODBUSMaster.data[i].dataType = discreteInput;
		MODBUSMaster.data[i].reg = modbusSwapEndian( ( *requestParser ).request02.firstInput ) + i;
		MODBUSMaster.data[i].value = modbusMaskRead( ( *parser ).response02.values, ( *parser ).response02.byteCount, i );

	}

	//Set up data length - response successfully parsed
	MODBUSMaster.dataLength = modbusSwapEndian( ( *requestParser ).request02.inputCount );
	MODBUSMaster.finished = 1;

	return 0;
}
