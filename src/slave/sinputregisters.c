#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/slave/stypes.h"
#include "../../include/modlib/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;

uint8_t MODBUSBuildResponse04( union MODBUSParser *Parser )
{
	//Response for master request04
	//Frame length (with CRC) is: 5 + ( ( *Parser ).Request04.RegisterCount << 1 )
	//5 bytes of data and each register * 2b ( 1 + 1 + 1 + 2x + 2 )
	uint8_t FrameLength = 5 + ( ( *Parser ).Request04.RegisterCount << 1 );
	uint8_t i = 0;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		return 0;
	}
	
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength ); //Allocate memory for builder union
	if ( Builder == NULL )
	{
		free( Builder );
		return MODBUS_ERROR_ALLOC;
	}

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( Builder );
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
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
	MODBUSSlave.Finished = 1;

	//Free union memory
	free( Builder );
	return 0;
}

uint8_t MODBUSParseRequest04( union MODBUSParser *Parser )
{
	//Read multiple input registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request04.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Ignore read request if frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return 0;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request04.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request04.FirstRegister );
	( *Parser ).Request04.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request04.RegisterCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request04.RegisterCount == 0 )
	{
		//Illegal data value error
		return MODBUSBuildException( 0x04, 0x03 );
	}

	if ( ( *Parser ).Request04.RegisterCount > MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address error
		return MODBUSBuildException( 0x04, 0x02 );
	}

	if ( ( *Parser ).Request04.FirstRegister >= MODBUSSlave.InputRegisterCount || (uint32_t) ( *Parser ).Request04.FirstRegister + (uint32_t) ( *Parser ).Request04.RegisterCount > (uint32_t) MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address exception
		return MODBUSBuildException( 0x04, 0x02 );
	}

	//Respond
	return MODBUSBuildResponse04( Parser );
}
