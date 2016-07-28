#include "lightmodbus/core.h"
#include "lightmodbus/slave.h"
#include "lightmodbus/parser.h"
#include "lightmodbus/slave/stypes.h"
#include "lightmodbus/slave/sregisters.h"
#include "lightmodbus/slave/scoils.h"
#include "lightmodbus/slave/sdiscreteinputs.h"
#include "lightmodbus/slave/sinputregisters.h"

uint8_t modbusBuildException( ModbusSlaveStatus *status, uint8_t function, uint8_t exceptionCode )
{
	//Generates modbus exception frame in allocated memory frame
	//Returns generated frame length

	//Reallocate frame memory
	status->response.frame = (uint8_t *) realloc( status->response.frame, 5 );
	if ( status->response.frame == NULL )
	{
		free( status->response.frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( status->response.frame, 0, 5 );
	union ModbusParser *exception = (union ModbusParser *) status->response.frame;

	//Setup exception frame
	( *exception ).exception.address = status->address;
	( *exception ).exception.function = ( 1 << 7 ) | function;
	( *exception ).exception.exceptionCode = exceptionCode;
	( *exception ).exception.crc = modbusCRC( ( *exception ).frame, 3 );

	//Set frame length - frame is ready
	status->response.length = 5;
	status->finished = 1;

	//So, user should rather know, that master is retarted, right?
	//That's the reason exception should be thrown - just like that, an information
	return MODBUS_ERROR_EXCEPTION;
}

uint8_t modbusParseRequest( ModbusSlaveStatus *status, uint8_t *frame, uint8_t frameLength )
{
	//Parse and interpret given modbus frame on slave-side

	//Init parser union
	//This one is actually unsafe, so it's easy to create a segmentation fault, so be careful here
	//Allowed frame array size in union is 256, but here I'm only allocating amount of frame length
	//It is even worse, compiler won't warn you, when you are outside the range
	//It works, and it uses much less memory, so I guess a bit of risk is fine in this case
	//Also, user needs to free memory alocated for frame himself!

	//Note: crc is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future crc can be verified right here

	uint8_t err = 0;

	//Reset response frame status
	status->response.length = 0;
	status->finished = 0;

	//If user tries to parse an empty frame return error (to avoid problems with memory allocation)
	if ( frameLength == 0 ) return MODBUS_ERROR_OTHER;

	//This part right there, below should be optimized, but currently I'm not 100% sure, that parsing doesn't malform given frame
	//In this case it's just much easier to allocate new frame
	union ModbusParser *parser = (union ModbusParser *) malloc( frameLength );
	if ( parser == NULL )
	{
		free( parser );
		return MODBUS_ERROR_ALLOC;
	}

	memcpy( parser->frame, frame, frameLength );

	//If frame is not broadcasted and address doesn't match skip parsing
	if ( parser->base.address != status->address && parser->base.address != 0 )
	{
		free( parser );
		status->finished = 1;
		return 0;
	}

	switch ( parser->base.function )
	{
		case 1: //Read multiple coils
			if ( LIGHTMODBUS_SLAVE_COILS ) err = modbusParseRequest01( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 2: //Read multiple discrete inputs
			if ( LIGHTMODBUS_SLAVE_DISCRETE_INPUTS ) err = modbusParseRequest02( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 3: //Read multiple holding registers
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) err = modbusParseRequest03( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 4: //Read multiple input registers
			if ( LIGHTMODBUS_SLAVE_INPUT_REGISTERS ) err = modbusParseRequest04( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 5: //Write single coil
			if ( LIGHTMODBUS_SLAVE_COILS ) err = modbusParseRequest05( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 6: //Write single holding reg
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) err = modbusParseRequest06( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 15: //Write multiple coils
			if ( LIGHTMODBUS_SLAVE_COILS ) err = modbusParseRequest15( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		case 16: //Write multiple holding registers
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) err = modbusParseRequest16( status, parser );
			else err = MODBUS_ERROR_PARSE;
			break;

		default:
			err = MODBUS_ERROR_PARSE;
			break;
	}

	if ( err == MODBUS_ERROR_PARSE )
		if ( parser->base.address != 0 ) err = modbusBuildException( status, parser->base.function, 0x01 );

	free( parser );

	return err;
}

uint8_t modbusSlaveInit( ModbusSlaveStatus *status )
{
	//Very basic init of slave side
	//User has to modify pointers etc. himself

	//Reset response frame status
	status->finished = 0;
	status->response.length = 0;
	status->response.frame = (uint8_t *) malloc( 8 );

	return ( ( status->response.frame == NULL ) * MODBUS_ERROR_ALLOC ) | ( ( status->address == 0 ) * MODBUS_ERROR_OTHER );
}

void modbusSlaveEnd( ModbusSlaveStatus *status )
{
	//Free memory
	free( status->response.frame );
}
