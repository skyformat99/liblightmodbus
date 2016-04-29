#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/slave/stypes.h"
#include "../../include/modlib/slave/sdiscreteinputs.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;

void MODBUSBuildResponse02( union MODBUSParser *Parser )
{
	//Response for master request02
	uint8_t FrameLength = 6 + ( ( ( *Parser ).Request02.InputCount - 1 ) >> 3 );
	uint8_t i = 0;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return;

	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength ); //Allocate memory for builder union
	memset( ( *Builder ).Frame, 0, FrameLength ); //Fill frame with zeros

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame

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

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Builder ).Frame, FrameLength );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;

	//Free union memory
	free( Builder );
}

void MODBUSParseRequest02( union MODBUSParser *Parser )
{
	//Read multiple discrete inputs
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request02.CRC ) return;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request02.FirstInput = MODBUSSwapEndian( ( *Parser ).Request02.FirstInput );
	( *Parser ).Request02.InputCount = MODBUSSwapEndian( ( *Parser ).Request02.InputCount );

	//Check if discrete input is in valid range
	if ( ( *Parser ).Request02.InputCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x02, 0x03 );
		return;
	}

	if ( ( *Parser ).Request02.InputCount > MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x02, 0x02 );
		return;
	}

	if ( ( *Parser ).Request02.FirstInput >= MODBUSSlave.DiscreteInputCount || (uint32_t) ( *Parser ).Request02.FirstInput + (uint32_t) ( *Parser ).Request02.InputCount > (uint32_t) MODBUSSlave.DiscreteInputCount )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x02, 0x02 );
		return;
	}

	//Respond
	MODBUSBuildResponse02( Parser );
}
