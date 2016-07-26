#include "lightmodbus/core.h"
#include "lightmodbus/parser.h"
#include "lightmodbus/slave/stypes.h"
#include "lightmodbus/slave/sregisters.h"

uint8_t modbusParseRequest04( ModbusSlaveStatus *status, union ModbusParser *parser )
{
	//Read multiple input registers
	//Using data from union pointer

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t i = 0;

	//Check frame crc
	if ( modbusCRC( parser->frame, frameLength - 2 ) != parser->request04.crc )
	{
		status->finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Don't do anything when frame is broadcasted
	if ( parser->base.address == 0 )
	{
		status->finished = 1;
		return 0;
	}

	//Swap endianness of longer members (but not crc)
	parser->request04.firstRegister = modbusSwapEndian( parser->request04.firstRegister );
	parser->request04.registerCount = modbusSwapEndian( parser->request04.registerCount );

	//Check if reg is in valid range
	if ( parser->request04.registerCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( status, 0x04, 0x03 );
	}

	if ( parser->request04.registerCount > status->InputRegisterCount )
	{
		//Illegal data address error
		return modbusBuildException( status, 0x04, 0x02 );
	}

	if ( parser->request04.firstRegister >= status->InputRegisterCount || (uint32_t) parser->request04.firstRegister + (uint32_t) parser->request04.registerCount > (uint32_t) status->InputRegisterCount )
	{
		//Illegal data address exception
		return modbusBuildException( status, 0x04, 0x02 );
	}

	//Respond
	frameLength = 5 + ( parser->request04.registerCount << 1 );

	status->response.frame = (uint8_t *) realloc( status->response.frame, frameLength ); //Reallocate response frame memory to needed memory
	if ( status->response.frame == NULL )
	{
		free( status->response.frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( status->response.frame, 0, frameLength ); //Empty response frame
	union ModbusParser *builder = (union ModbusParser *) status->response.frame; //Allocate memory for builder union

	//Set up basic response data
	builder->response04.address = status->address;
	builder->response04.function = parser->request04.function;
	builder->response04.byteCount = parser->request04.registerCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < parser->request04.registerCount; i++ )
		builder->response04.values[i] = modbusSwapEndian( status->InputRegisters[parser->request04.firstRegister + i] );

	//Calculate crc
	builder->response04.values[parser->request04.registerCount] = modbusCRC( builder->frame, frameLength - 2 );

	//Set frame length - frame is ready
	status->response.length = frameLength;
	status->finished = 1;

	return 0;
}
