#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/scoils.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t modbusParseRequest01( union MODBUSParser *Parser )
{
	//Read multiple coils
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request01.CRC )
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
	( *Parser ).Request01.FirstCoil = modbusSwapEndian( ( *Parser ).Request01.FirstCoil );
	( *Parser ).Request01.CoilCount = modbusSwapEndian( ( *Parser ).Request01.CoilCount );

	//Check if coil is in valid range
	if ( ( *Parser ).Request01.CoilCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x01, 0x03 );
	}

	if ( ( *Parser ).Request01.CoilCount > MODBUSSlave.CoilCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x01, 0x02 );
	}

	if ( ( *Parser ).Request01.FirstCoil >= MODBUSSlave.CoilCount || (uint32_t) ( *Parser ).Request01.FirstCoil + (uint32_t) ( *Parser ).Request01.CoilCount > (uint32_t) MODBUSSlave.CoilCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x01, 0x02 );
	}

	//Respond
	FrameLength = 6 + ( ( ( *Parser ).Request01.CoilCount - 1 ) >> 3 );

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
	( *Builder ).Response01.BytesCount = 1 + ( ( ( *Parser ).Request01.CoilCount - 1 ) >> 3 );

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request01.CoilCount; i++ )
		modbusMaskWrite( ( *Builder ).Response01.Values, 32, i, modbusMaskRead( MODBUSSlave.Coils, 1 + ( ( MODBUSSlave.CoilCount - 1 ) >> 3 ), i + ( *Parser ).Request01.FirstCoil ) );

	//Calculate CRC
	( *Builder ).Frame[FrameLength - 2] = modbusCRC( ( *Builder ).Frame, FrameLength - 2 ) & 0x00FF;
	( *Builder ).Frame[FrameLength - 1] = ( modbusCRC( ( *Builder ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8;

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t modbusParseRequest05( union MODBUSParser *Parser )
{
	//Write single coil
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request01.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request05.Coil = modbusSwapEndian( ( *Parser ).Request05.Coil );
	( *Parser ).Request05.Value = modbusSwapEndian( ( *Parser ).Request05.Value );

	//Check if coil is in valid range
	if ( ( *Parser ).Request05.Coil >= MODBUSSlave.CoilCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x05, 0x02 );
		return 0;
	}

	//Check if coil value is valid
	if ( ( *Parser ).Request05.Value != 0x0000 && ( *Parser ).Request05.Value != 0xFF00 )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x05, 0x03 );
		return 0;
	}

	//Respond
	FrameLength = 8;


	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//After all possible exceptions, write coils
	modbusMaskWrite( MODBUSSlave.Coils, 1 + ( ( MODBUSSlave.CoilCount - 1 ) << 3 ), ( *Parser ).Request05.Coil, ( *Parser ).Request05.Value == 0xFF00 );

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *Builder ).Base.Address = MODBUSSlave.Address;
	( *Builder ).Base.Function = ( *Parser ).Base.Function;
	( *Builder ).Response05.Coil = modbusSwapEndian( ( *Parser ).Request05.Coil );
	( *Builder ).Response05.Value = modbusSwapEndian( ( *Parser ).Request05.Value );

	//Calculate CRC
	( *Builder ).Response05.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t modbusParseRequest15( union MODBUSParser *Parser )
{
	//Write multiple coils
	//Using data from union pointer

	//Update frame length
	uint8_t i = 0;
	uint8_t FrameLength = 9 + ( *Parser ).Request15.BytesCount;

	//Check frame CRC
	//Shifting is used instead of dividing for optimisation on smaller devices (AVR)
	if ( ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) & 0x00FF ) != ( *Parser ).Request15.Values[( *Parser ).Request15.BytesCount] )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	if ( ( ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) & 0xFF00 ) >> 8 ) != ( *Parser ).Request15.Values[( *Parser ).Request15.BytesCount + 1] )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check if bytes or registers count isn't 0
	if ( ( *Parser ).Request15.BytesCount == 0 || ( *Parser ).Request15.CoilCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x0F, 0x03 );
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request15.FirstCoil = modbusSwapEndian( ( *Parser ).Request15.FirstCoil );
	( *Parser ).Request15.CoilCount = modbusSwapEndian( ( *Parser ).Request15.CoilCount );

	//Check if bytes count matches coils count
	if ( 1 + ( ( ( *Parser ).Request15.CoilCount - 1 ) >> 3 )  != ( *Parser ).Request15.BytesCount )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x0F, 0x03 );
		return 0;
	}

	//Check if registers are in valid range
	if ( ( *Parser ).Request15.CoilCount > MODBUSSlave.CoilCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x0F, 0x02 );
		return 0;
	}

	if ( ( *Parser ).Request15.FirstCoil >= MODBUSSlave.CoilCount || (uint32_t) ( *Parser ).Request15.FirstCoil + (uint32_t) ( *Parser ).Request15.CoilCount > (uint32_t) MODBUSSlave.CoilCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x0F, 0x02 );
		return 0;
	}

	//Respond
	FrameLength = 8;

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSSlave.Response.Frame; //Allocate memory for builder union

	//After all possible exceptions write values to registers
	for ( i = 0; i < ( *Parser ).Request15.CoilCount; i++ )
		modbusMaskWrite( MODBUSSlave.Coils, MODBUSSlave.CoilCount, ( *Parser ).Request15.FirstCoil + i, modbusMaskRead( ( *Parser ).Request15.Values, ( *Parser ).Request15.BytesCount, i ) );

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *Builder ).Base.Address = MODBUSSlave.Address;
	( *Builder ).Base.Function = ( *Parser ).Base.Function;
	( *Builder ).Response15.FirstCoil = modbusSwapEndian( ( *Parser ).Request15.FirstCoil );
	( *Builder ).Response15.CoilCount = modbusSwapEndian( ( *Parser ).Request15.CoilCount );

	//Calculate CRC
	( *Builder ).Response15.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
