#include "lightmodbus/core.h"
#include "lightmodbus/parser.h"
#include "lightmodbus/master/mtypes.h"
#include "lightmodbus/master/mdiscreteinputs.h"

uint8_t modbusBuildRequest02( ModbusMasterStatus *status, uint8_t address, uint16_t firstInput, uint16_t inputCount )
{
	//Build request02 frame, to send it so slave
	//Read multiple discrete inputs

	//Set frame length
	uint8_t frameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	status->request.length = 0;
	status->finished = 0;

	//Reallocate memory for final frame
	status->request.frame = (uint8_t *) realloc( status->request.frame, frameLength );
	if ( status->request.frame == NULL )
	{
		free( status->request.frame );
		return MODBUS_ERROR_ALLOC;
	}
	union ModbusParser *builder = (union ModbusParser *) status->request.frame;

	builder->base.address = address;
	builder->base.function = 2;
	builder->request02.firstInput = modbusSwapEndian( firstInput );
	builder->request02.inputCount = modbusSwapEndian( inputCount );

	//Calculate crc
	builder->request02.crc = modbusCRC( builder->frame, frameLength - 2 );

	status->request.length = frameLength;
	status->finished = 1;

	return 0;
}

uint8_t modbusParseResponse02( ModbusMasterStatus *status, union ModbusParser *parser, union ModbusParser *requestParser )
{
	//Parse slave response to request 02 (read multiple discrete inputs)

	//Update frame length
	uint8_t frameLength = 5 + parser->response02.byteCount;
	uint8_t dataok = 1;
	uint8_t i = 0;

	//Check frame crc
	dataok &= ( modbusCRC( parser->frame, frameLength - 2 ) & 0x00FF ) == parser->response02.values[parser->response02.byteCount];
	dataok &= ( ( modbusCRC( parser->frame, frameLength - 2 ) & 0xFF00 ) >> 8 ) == parser->response02.values[parser->response02.byteCount + 1];

	if ( !dataok )
	{
		status->finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check between data sent to slave and received from slave
	dataok &= ( parser->base.address == requestParser->base.address );
	dataok &= ( parser->base.function == requestParser->base.function );


	status->data = (ModbusData *) realloc( status->data, sizeof( ModbusData ) * modbusSwapEndian( requestParser->request02.inputCount ) );
	for ( i = 0; i < modbusSwapEndian( requestParser->request02.inputCount ); i++ )
	{
		status->data[i].address = parser->base.address;
		status->data[i].dataType = discreteInput;
		status->data[i].reg = modbusSwapEndian( requestParser->request02.firstInput ) + i;
		status->data[i].value = modbusMaskRead( parser->response02.values, parser->response02.byteCount, i );

	}

	//Set up data length - response successfully parsed
	status->dataLength = modbusSwapEndian( requestParser->request02.inputCount );
	status->finished = 1;

	return 0;
}
