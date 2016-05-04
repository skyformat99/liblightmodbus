#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/slave/stypes.h"
#include "../../include/modlib/slave/sdiscreteinputs.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;


uint8_t MODBUSParseRequest02( union MODBUSParser *Parser )
{
	//Read multiple discrete inputs
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request02.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Don't do anything when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request02.FirstInput = MODBUSSwapEndian( ( *Parser ).Request02.FirstInput );
	( *Parser ).Request02.InputCount = MODBUSSwapEndian( ( *Parser ).Request02.InputCount );

	//Check if discrete input is in valid range
	if ( ( *Parser ).Request02.InputCount == 0 )
	{
		//Illegal data value error
		return MODBUSBuildException( 0x02, 0x03 );
	}

	if ( ( *Parser ).Request02.InputCount > MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address error
		return MODBUSBuildException( 0x02, 0x02 );
	}

	if ( ( *Parser ).Request02.FirstInput >= MODBUSSlave.DiscreteInputCount || (uint32_t) ( *Parser ).Request02.FirstInput + (uint32_t) ( *Parser ).Request02.InputCount > (uint32_t) MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address exception
		return MODBUSBuildException( 0x02, 0x02 );
	}

	//Respond
	FrameLength = 6 + ( ( ( *Parser ).Request02.InputCount - 1 ) >> 3 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//Set up basic response data
	( *Builder ).Base.Address = MODBUSSlave.Address;
	( *Builder ).Base.Function = ( *Parser ).Base.Function;
	( *Builder ).Response02.BytesCount = 1 + ( ( ( *Parser ).Request02.InputCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request02.InputCount; i++ )
		MODBUSWriteMaskBit( ( *Builder ).Response02.Values, 32, i, MODBUSReadMaskBit( MODBUSSlave.DiscreteInputs, 1 + ( ( MODBUSSlave.DiscreteInputCount - 1 ) >> 3 ), i + ( *Parser ).Request02.FirstInput ) );

	//Calculate CRC
	( *Builder ).Frame[FrameLength - 2] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 ) & 0x00FF;
	( *Builder ).Frame[FrameLength - 1] = ( MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8;

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
