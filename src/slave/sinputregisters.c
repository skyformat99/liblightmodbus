#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t modbusParseRequest04( union MODBUSParser *parser )
{
	//Read multiple input registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Request04.CRC )
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
	( *parser ).Request04.FirstRegister = modbusSwapEndian( ( *parser ).Request04.FirstRegister );
	( *parser ).Request04.RegisterCount = modbusSwapEndian( ( *parser ).Request04.RegisterCount );

	//Check if register is in valid range
	if ( ( *parser ).Request04.RegisterCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x04, 0x03 );
	}

	if ( ( *parser ).Request04.RegisterCount > MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x04, 0x02 );
	}

	if ( ( *parser ).Request04.FirstRegister >= MODBUSSlave.InputRegisterCount || (uint32_t) ( *parser ).Request04.FirstRegister + (uint32_t) ( *parser ).Request04.RegisterCount > (uint32_t) MODBUSSlave.InputRegisterCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x04, 0x02 );
	}

	//Respond
	FrameLength = 5 + ( ( *parser ).Request04.RegisterCount << 1 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSSlave.Response.Frame; //Allocate memory for builder union

	//Set up basic response data
	( *builder ).Response04.Address = MODBUSSlave.Address;
	( *builder ).Response04.Function = ( *parser ).Request04.Function;
	( *builder ).Response04.BytesCount = ( *parser ).Request04.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *parser ).Request04.RegisterCount; i++ )
		( *builder ).Response04.Values[i] = modbusSwapEndian( MODBUSSlave.InputRegisters[( *parser ).Request04.FirstRegister + i] );

	//Calculate CRC
	( *builder ).Response04.Values[( *parser ).Request04.RegisterCount] = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
