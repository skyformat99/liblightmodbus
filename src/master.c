#include "lightmodbus/master.h"
#include "lightmodbus/core.h"
#include "lightmodbus/parser.h"
#include "lightmodbus/master/mtypes.h"
#include "lightmodbus/master/mregisters.h"
#include "lightmodbus/master/mcoils.h"
#include "lightmodbus/master/mdiscreteinputs.h"
#include "lightmodbus/master/minputregisters.h"

uint8_t modbusParseException( ModbusMasterStatus *status, union ModbusParser *parser )
{
	//Parse exception frame and write data to MODBUSMaster structure

	//Check crc
	if ( modbusCRC( parser->frame, 3 ) != parser->exception.crc )
	{
		status->finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Copy data
	status->exception.address = parser->exception.address;
	status->exception.function = parser->exception.function;
	status->exception.Code = parser->exception.exceptionCode;

	status->finished = 1;

	return MODBUS_ERROR_EXCEPTION;
}

uint8_t modbusParseResponse( ModbusMasterStatus *status, uint8_t *frame, uint8_t frameLength, uint8_t *RequestFrame, uint8_t RequestFrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Note: crc is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future crc can be verified right here

	//If non-zero some parser failed its job
	uint8_t err = 0;

	//Reset output registers before parsing frame
	status->dataLength = 0;
	status->exception.address = 0;
	status->exception.function = 0;
	status->exception.Code = 0;
	status->finished = 0;

	//If user tries to parse an empty frame return error (to avoid problems with memory allocation)
	if ( frameLength == 0 ) return MODBUS_ERROR_OTHER;

	//Allocate memory for union and copy frame to it
	union ModbusParser *parser = (union ModbusParser *) malloc( frameLength );
	if ( parser == NULL )
	{
		free( parser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( parser->frame,  frame, frameLength );

	//Allocate memory for request union and copy frame to it
	union ModbusParser *requestParser = (union ModbusParser *) malloc( RequestFrameLength );
	if ( requestParser == NULL )
	{
		free( parser );
		free( requestParser );
		return MODBUS_ERROR_ALLOC;
	}
	memcpy( requestParser->frame,  RequestFrame, RequestFrameLength );

	//Check if frame is exception response
	if ( parser->base.function & 128 )
	{
		err = modbusParseException( status, parser );
	}
	else
	{
		switch ( parser->base.function )
		{
			case 1: //Read multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse01( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 2: //Read multiple discrete inputs
				if ( LIGHTMODBUS_MASTER_DISCRETE_INPUTS ) err = modbusParseResponse02( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 3: //Read multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse03( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 4: //Read multiple input registers
				if ( LIGHTMODBUS_MASTER_INPUT_REGISTERS ) err = modbusParseResponse04( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 5: //Write single coil
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse05( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 6: //Write single holding reg
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse06( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 15: //Write multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse15( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 16: //Write multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse16( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			default: //function code not known by master
				err = MODBUS_ERROR_PARSE;
				break;
		}
	}

	//Free used memory
	free( parser );
	free( requestParser );

	return err;
}

uint8_t modbusMasterInit( ModbusMasterStatus *status )
{
	//Very basic init of master side
	status->request.frame = (uint8_t *) malloc( 8 );
	status->request.length = 0;
	status->data = (ModbusData *) malloc( sizeof( ModbusData ) );
	status->dataLength = 0;
	status->finished = 0;

	status->exception.address = 0;
	status->exception.function = 0;
	status->exception.Code = 0;

	return ( ( status->request.frame == NULL ) || ( status->data == NULL ) ) * MODBUS_ERROR_ALLOC;
}

void modbusMasterEnd( ModbusMasterStatus *status )
{
	//Free memory
	free( status->request.frame );
	free( status->data );
}
