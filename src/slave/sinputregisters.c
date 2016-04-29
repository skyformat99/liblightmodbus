#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/slave/stypes.h"
#include "../../include/modlib/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;

void MODBUSBuildResponse04( union MODBUSParser *Parser )
{
	//Response for master request04
	//Frame length (with CRC) is: 5 + ( ( *Parser ).Request04.RegisterCount << 1 )
	//5 bytes of data and each register * 2b ( 1 + 1 + 1 + 2x + 2 )
	uint8_t FrameLength = 5 + ( ( *Parser ).Request04.RegisterCount << 1 );
	uint8_t i = 0;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return;

	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength ); //Allocate memory for builder union
	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame

	//Set up basic response data
	( *Builder ).Response04.Address = MODBUSSlave.Address;
	( *Builder ).Response04.Function = ( *Parser ).Request04.Function;
	( *Builder ).Response04.BytesCount = ( *Parser ).Request04.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request04.RegisterCount; i++ )
		( *Builder ).Response04.Values[i] = MODBUSSwapEndian( MODBUSSlave.InputRegisters[( *Parser ).Request04.FirstRegister + i] );

	//Calculate CRC
	( *Builder ).Response04.Values[( *Parser ).Request04.RegisterCount] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Builder ).Frame, FrameLength );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;

	//Free union memory
	free( Builder );
}

void MODBUSParseRequest04( union MODBUSParser *Parser )
{
	//Read multiple input registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request04.CRC ) return;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request04.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request04.FirstRegister );
	( *Parser ).Request04.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request04.RegisterCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request04.RegisterCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x04, 0x03 );
		return;
	}

	if ( ( *Parser ).Request04.RegisterCount > MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x04, 0x02 );
		return;
	}

	if ( ( *Parser ).Request04.FirstRegister >= MODBUSSlave.InputRegisterCount || (uint32_t) ( *Parser ).Request04.FirstRegister + (uint32_t) ( *Parser ).Request04.RegisterCount > (uint32_t) MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( 0x04, 0x02 );
		return;
	}

	//Respond
	MODBUSBuildResponse04( Parser );
}
