#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sdiscreteinputs.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;


uint8_t modbusParseRequest02( union MODBUSParser *parser )
{
	//Read multiple discrete inputs
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Request02.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Don't do anything when frame is broadcasted
	if ( ( *parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *parser ).Request02.FirstInput = modbusSwapEndian( ( *parser ).Request02.FirstInput );
	( *parser ).Request02.InputCount = modbusSwapEndian( ( *parser ).Request02.InputCount );

	//Check if discrete input is in valid range
	if ( ( *parser ).Request02.InputCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x02, 0x03 );
	}

	if ( ( *parser ).Request02.InputCount > MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x02, 0x02 );
	}

	if ( ( *parser ).Request02.FirstInput >= MODBUSSlave.DiscreteInputCount || (uint32_t) ( *parser ).Request02.FirstInput + (uint32_t) ( *parser ).Request02.InputCount > (uint32_t) MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x02, 0x02 );
	}

	//Respond
	FrameLength = 6 + ( ( ( *parser ).Request02.InputCount - 1 ) >> 3 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//Set up basic response data
	( *builder ).Base.Address = MODBUSSlave.Address;
	( *builder ).Base.Function = ( *parser ).Base.Function;
	( *builder ).Response02.BytesCount = 1 + ( ( ( *parser ).Request02.InputCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *parser ).Request02.InputCount; i++ )
		modbusMaskWrite( ( *builder ).Response02.Values, 32, i, modbusMaskRead( MODBUSSlave.DiscreteInputs, 1 + ( ( MODBUSSlave.DiscreteInputCount - 1 ) >> 3 ), i + ( *parser ).Request02.FirstInput ) );

	//Calculate CRC
	( *builder ).Frame[FrameLength - 2] = modbusCRC( ( *builder ).Frame, FrameLength - 2 ) & 0x00FF;
	( *builder ).Frame[FrameLength - 1] = ( modbusCRC( ( *builder ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8;

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
