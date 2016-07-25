#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/scoils.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t modbusParseRequest01( union ModbusParser *parser )
{
	//Read multiple coils
	//Using data from union pointer

	//Update frame length
	uint8_t frameLength = 8;
	uint8_t i = 0;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).request01.crc )
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
	( *parser ).request01.firstCoil = modbusSwapEndian( ( *parser ).request01.firstCoil );
	( *parser ).request01.coilCount = modbusSwapEndian( ( *parser ).request01.coilCount );

	//Check if coil is in valid range
	if ( ( *parser ).request01.coilCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x01, 0x03 );
	}

	if ( ( *parser ).request01.coilCount > MODBUSSlave.coilCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x01, 0x02 );
	}

	if ( ( *parser ).request01.firstCoil >= MODBUSSlave.coilCount || (uint32_t) ( *parser ).request01.firstCoil + (uint32_t) ( *parser ).request01.coilCount > (uint32_t) MODBUSSlave.coilCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x01, 0x02 );
	}

	//Respond
	frameLength = 6 + ( ( ( *parser ).request01.coilCount - 1 ) >> 3 );

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
	( *builder ).response01.byteCount = 1 + ( ( ( *parser ).request01.coilCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *parser ).request01.coilCount; i++ )
		modbusMaskWrite( ( *builder ).response01.values, 32, i, modbusMaskRead( MODBUSSlave.Coils, 1 + ( ( MODBUSSlave.coilCount - 1 ) >> 3 ), i + ( *parser ).request01.firstCoil ) );

	//Calculate crc
	( *builder ).frame[frameLength - 2] = modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0x00FF;
	( *builder ).frame[frameLength - 1] = ( modbusCRC( ( *builder ).frame, frameLength - 2 ) & 0xFF00 ) >> 8;

	//Set frame length - frame is ready
	MODBUSSlave.response.length = frameLength;
	MODBUSSlave.finished = 1;

	return 0;
}

uint8_t modbusParseRequest05( union ModbusParser *parser )
{
	//Write single coil
	//Using data from union pointer

	//Update frame length
	uint8_t frameLength = 8;

	//Check frame crc
	if ( modbusCRC( ( *parser ).frame, frameLength - 2 ) != ( *parser ).request01.crc )
	{
		MODBUSSlave.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Swap endianness of longer members (but not crc)
	( *parser ).request05.coil = modbusSwapEndian( ( *parser ).request05.coil );
	( *parser ).request05.value = modbusSwapEndian( ( *parser ).request05.value );

	//Check if coil is in valid range
	if ( ( *parser ).request05.coil >= MODBUSSlave.coilCount )
	{
		//Illegal data address error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x05, 0x02 );
		return 0;
	}

	//Check if coil value is valid
	if ( ( *parser ).request05.value != 0x0000 && ( *parser ).request05.value != 0xFF00 )
	{
		//Illegal data address error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x05, 0x03 );
		return 0;
	}

	//Respond
	frameLength = 8;


	MODBUSSlave.response.frame = (uint8_t *) realloc( MODBUSSlave.response.frame, frameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.response.frame == NULL )
	{
		free( MODBUSSlave.response.frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.response.frame, 0, frameLength ); //Empty response frame
	union ModbusParser *builder = (union ModbusParser *) MODBUSSlave.response.frame;

	//After all possible exceptions, write coils
	modbusMaskWrite( MODBUSSlave.Coils, 1 + ( ( MODBUSSlave.coilCount - 1 ) << 3 ), ( *parser ).request05.coil, ( *parser ).request05.value == 0xFF00 );

	//Do not respond when frame is broadcasted
	if ( ( *parser ).base.address == 0 )
	{
		MODBUSSlave.finished = 1;
		return 0;
	}

	//Set up basic response data
	( *builder ).base.address = MODBUSSlave.address;
	( *builder ).base.function = ( *parser ).base.function;
	( *builder ).response05.coil = modbusSwapEndian( ( *parser ).request05.coil );
	( *builder ).response05.value = modbusSwapEndian( ( *parser ).request05.value );

	//Calculate crc
	( *builder ).response05.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.response.length = frameLength;
	MODBUSSlave.finished = 1;

	return 0;
}

uint8_t modbusParseRequest15( union ModbusParser *parser )
{
	//Write multiple coils
	//Using data from union pointer

	//Update frame length
	uint8_t i = 0;
	uint8_t frameLength = 9 + ( *parser ).request15.byteCount;

	//Check frame crc
	//Shifting is used instead of dividing for optimisation on smaller devices (AVR)
	if ( ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0x00FF ) != ( *parser ).request15.values[( *parser ).request15.byteCount] )
	{
		MODBUSSlave.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	if ( ( ( modbusCRC( ( *parser ).frame, frameLength - 2 ) & 0xFF00 ) >> 8 ) != ( *parser ).request15.values[( *parser ).request15.byteCount + 1] )
	{
		MODBUSSlave.finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check if bytes or registers count isn't 0
	if ( ( *parser ).request15.byteCount == 0 || ( *parser ).request15.coilCount == 0 )
	{
		//Illegal data value error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x0F, 0x03 );
		return 0;
	}

	//Swap endianness of longer members (but not crc)
	( *parser ).request15.firstCoil = modbusSwapEndian( ( *parser ).request15.firstCoil );
	( *parser ).request15.coilCount = modbusSwapEndian( ( *parser ).request15.coilCount );

	//Check if bytes count matches coils count
	if ( 1 + ( ( ( *parser ).request15.coilCount - 1 ) >> 3 )  != ( *parser ).request15.byteCount )
	{
		//Illegal data value error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x0F, 0x03 );
		return 0;
	}

	//Check if registers are in valid range
	if ( ( *parser ).request15.coilCount > MODBUSSlave.coilCount )
	{
		//Illegal data address error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x0F, 0x02 );
		return 0;
	}

	if ( ( *parser ).request15.firstCoil >= MODBUSSlave.coilCount || (uint32_t) ( *parser ).request15.firstCoil + (uint32_t) ( *parser ).request15.coilCount > (uint32_t) MODBUSSlave.coilCount )
	{
		//Illegal data address error
		if ( ( *parser ).base.address != 0 ) return modbusBuildException( 0x0F, 0x02 );
		return 0;
	}

	//Respond
	frameLength = 8;

	MODBUSSlave.response.frame = (uint8_t *) realloc( MODBUSSlave.response.frame, frameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.response.frame == NULL )
	{
		free( MODBUSSlave.response.frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.response.frame, 0, frameLength ); //Empty response frame
	union ModbusParser *builder = (union ModbusParser *) MODBUSSlave.response.frame; //Allocate memory for builder union

	//After all possible exceptions write values to registers
	for ( i = 0; i < ( *parser ).request15.coilCount; i++ )
		modbusMaskWrite( MODBUSSlave.Coils, MODBUSSlave.coilCount, ( *parser ).request15.firstCoil + i, modbusMaskRead( ( *parser ).request15.values, ( *parser ).request15.byteCount, i ) );

	//Do not respond when frame is broadcasted
	if ( ( *parser ).base.address == 0 )
	{
		MODBUSSlave.finished = 1;
		return 0;
	}

	//Set up basic response data
	( *builder ).base.address = MODBUSSlave.address;
	( *builder ).base.function = ( *parser ).base.function;
	( *builder ).response15.firstCoil = modbusSwapEndian( ( *parser ).request15.firstCoil );
	( *builder ).response15.coilCount = modbusSwapEndian( ( *parser ).request15.coilCount );

	//Calculate crc
	( *builder ).response15.crc = modbusCRC( ( *builder ).frame, frameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.response.length = frameLength;
	MODBUSSlave.finished = 1;

	return 0;
}
