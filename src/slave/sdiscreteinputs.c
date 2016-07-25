#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sdiscreteinputs.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;


uint8_t modbusParseRequest02( union ModbusParser *parser )
{
	//Read multiple discrete inputs
	//Using data from union pointer

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t i = 0;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).request02.crc )
	{
		MODBUSSlave.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Don't do anything when frame is broadcasted
	if ( ( *parser ).base.address == 0 )
	{
		MODBUSSlave.finished = 1;
		return 0;
	}

	//Swap endianness of longer members (but not crc)
	( *parser ).request02.firstInput = modbusSwapEndian( ( *parser ).request02.firstInput );
	( *parser ).request02.inputCount = modbusSwapEndian( ( *parser ).request02.inputCount );

	//Check if discrete input is in valid range
	if ( ( *parser ).request02.inputCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x02, 0x03 );
	}

	if ( ( *parser ).request02.inputCount > MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x02, 0x02 );
	}

	if ( ( *parser ).request02.firstInput >= MODBUSSlave.DiscreteInputCount || (uint32_t) ( *parser ).request02.firstInput + (uint32_t) ( *parser ).request02.inputCount > (uint32_t) MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x02, 0x02 );
	}

	//Respond
	frameLength = 6 + ( ( ( *parser ).request02.inputCount - 1 ) >> 3 );

	MODBUSSlave.response.frame = (uint8_t *) realloc( MODBUSSlave.response.frame, frameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.response.frame == NULL )
	{
		free( MODBUSSlave.response.frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.response.frame, 0, frameLength ); //Empty response frame
	union ModbusParser *builder = (union ModbusParser *) MODBUSSlave.response.frame;

	//Set up basic response data
	( *builder ).base.address = MODBUSSlave.address;
	( *builder ).base.function = ( *parser ).base.function;
	( *builder ).response02.byteCount = 1 + ( ( ( *parser ).request02.inputCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *parser ).request02.inputCount; i++ )
		modbusMaskWrite( ( *builder ).response02.values, 32, i, modbusMaskRead( MODBUSSlave.DiscreteInputs, 1 + ( ( MODBUSSlave.DiscreteInputCount - 1 ) >> 3 ), i + ( *parser ).request02.firstInput ) );

	//Calculate crc
	( *builder ).frame[frameLength - 2] = modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0x00FF;
	( *builder ).frame[frameLength - 1] = ( modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0xFF00 ) >> 8;

	//Set frame length - frame is ready
	MODBUSSlave.response.length = frameLength;
	MODBUSSlave.finished = 1;

	return 0;
}
