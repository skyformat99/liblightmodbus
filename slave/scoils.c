#include "../modlib.h"
#include "../parser.h"
#include "stypes.h"

#include "scoils.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;

void MODBUSBuildResponse01( union MODBUSParser *Parser )
{
	//Response for master request01
	uint8_t FrameLength = 6 + ( ( ( *Parser ).Request01.CoilCount - 1 ) >> 3 );
	uint8_t i = 0;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return;

	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength ); //Allocate memory for builder union
	memset( ( *Builder ).Frame, 0, FrameLength ); //Fill frame with zeros
	
	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame

	//Set up basic response data
	( *Builder ).Response01.Address = MODBUSSlave.Address;
	( *Builder ).Response01.Function = ( *Parser ).Request03.Function;
	( *Builder ).Response01.BytesCount = 1 + ( ( ( *Parser ).Request01.CoilCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request01.CoilCount; i++ )
		MODBUSWriteMaskBit( ( *Builder ).Response01.Values, 32, i, MODBUSReadMaskBit( MODBUSSlave.Coils, 1 + ( ( MODBUSSlave.CoilCount - 1 ) >> 3 ), i + ( *Parser ).Request01.FirstCoil ) );

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

void MODBUSParseRequest01( union MODBUSParser *Parser )
{
	//Read multiple coils
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request01.CRC ) return;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request01.FirstCoil = MODBUSSwapEndian( ( *Parser ).Request01.FirstCoil );
	( *Parser ).Request01.CoilCount = MODBUSSwapEndian( ( *Parser ).Request01.CoilCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request01.CoilCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x10, 0x03 );
		return;
	}

	if ( ( *Parser ).Request01.CoilCount > MODBUSSlave.CoilCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x10, 0x02 );
		return;
	}

	if ( ( *Parser ).Request01.FirstCoil >= MODBUSSlave.CoilCount || (uint32_t) ( *Parser ).Request01.FirstCoil + (uint32_t) ( *Parser ).Request01.CoilCount > (uint32_t) MODBUSSlave.CoilCount )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x03, 0x02 );
		return;
	}

	//Respond
	MODBUSBuildResponse01( Parser );
}
