#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t MODBUSParseRequest04( union MODBUSParser *Parser )
{
	//Read multiple input registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request04.CRC )
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
	FrameLength = 5 + ( ( *Parser ).Request04.RegisterCount << 1 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSSlave.Response.Frame; //Allocate memory for builder union

	//Set up basic response data
	( *Builder ).Response04.Address = MODBUSSlave.Address;
	( *Builder ).Response04.Function = ( *Parser ).Request04.Function;
	( *Builder ).Response04.BytesCount = ( *Parser ).Request04.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request04.RegisterCount; i++ )
		( *Builder ).Response04.Values[i] = MODBUSSwapEndian( MODBUSSlave.InputRegisters[( *Parser ).Request04.FirstRegister + i] );

	//Calculate CRC
	( *Builder ).Response04.Values[( *Parser ).Request04.RegisterCount] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
