#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t MODBUSParseRequest03( union MODBUSParser *Parser )
{
	//Read multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request03.CRC )
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
	( *Parser ).Request03.FirstRegister = modbusSwapEndian( ( *Parser ).Request03.FirstRegister );
	( *Parser ).Request03.RegisterCount = modbusSwapEndian( ( *Parser ).Request03.RegisterCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request03.RegisterCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x03, 0x03 );
	}

	if ( ( *Parser ).Request03.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x03, 0x02 );
	}

	if ( ( *Parser ).Request03.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *Parser ).Request03.FirstRegister + (uint32_t) ( *Parser ).Request03.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x03, 0x02 );
	}

	//Respond
	FrameLength = 5 + ( ( *Parser ).Request03.RegisterCount << 1 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *Builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//Set up basic response data
	( *Builder ).Response03.Address = MODBUSSlave.Address;
	( *Builder ).Response03.Function = ( *Parser ).Request03.Function;
	( *Builder ).Response03.BytesCount = ( *Parser ).Request03.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request03.RegisterCount; i++ )
		( *Builder ).Response03.Values[i] = modbusSwapEndian( MODBUSSlave.Registers[( *Parser ).Request03.FirstRegister + i] );

	//Calculate CRC
	( *Builder ).Response03.Values[( *Parser ).Request03.RegisterCount] = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t MODBUSParseRequest06( union MODBUSParser *Parser )
{
	//Write single holding register
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request06.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request06.Register = modbusSwapEndian( ( *Parser ).Request06.Register );
	( *Parser ).Request06.Value = modbusSwapEndian( ( *Parser ).Request06.Value );

	//Check if register is in valid range
	if ( ( *Parser ).Request06.Register >= MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x06, 0x02 );
		return 0;
	}

	//Check if register is allowed to be written
	if ( modbusMaskRead( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *Parser ).Request06.Register ) == 1 )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x06, 0x02 );
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

	//After all possible exceptions, write register
	MODBUSSlave.Registers[( *Parser ).Request06.Register] = ( *Parser ).Request06.Value;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *Builder ).Response06.Address = MODBUSSlave.Address;
	( *Builder ).Response06.Function = ( *Parser ).Request06.Function;
	( *Builder ).Response06.Register = modbusSwapEndian( ( *Parser ).Request06.Register );
	( *Builder ).Response06.Value = modbusSwapEndian( MODBUSSlave.Registers[( *Parser ).Request06.Register] );

	//Calculate CRC
	( *Builder ).Response06.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t MODBUSParseRequest16( union MODBUSParser *Parser )
{
	//Write multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t i = 0;
	uint8_t MaskSum = 0;
	uint8_t FrameLength = 9 + ( *Parser ).Request16.BytesCount;

	//Check frame CRC
	//Shifting is used instead of dividing for optimisation on smaller devices (AVR)
	if ( modbusCRC( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request16.Values[( *Parser ).Request16.BytesCount >> 1] )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check if bytes or registers count isn't 0
	if ( ( *Parser ).Request16.BytesCount == 0 || ( *Parser ).Request16.RegisterCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x03 );
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request16.FirstRegister = modbusSwapEndian( ( *Parser ).Request16.FirstRegister );
	( *Parser ).Request16.RegisterCount = modbusSwapEndian( ( *Parser ).Request16.RegisterCount );

	//Check if bytes count *2 is equal to registers count
	if ( ( *Parser ).Request16.RegisterCount != ( ( *Parser ).Request16.BytesCount >> 1 ) )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x03 );
		return 0;
	}

	//Check if registers are in valid range
	if ( ( *Parser ).Request16.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
		return 0;
	}

	if ( ( *Parser ).Request16.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *Parser ).Request16.FirstRegister + (uint32_t) ( *Parser ).Request16.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
		return 0;
	}

	//Check for write protection
	for ( i = 0; i < ( *Parser ).Request16.RegisterCount; i++ )
		MaskSum += (  modbusMaskRead( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *Parser ).Request16.FirstRegister + i ) == 1 ) ? 1 : 0;

	if ( MaskSum > 0 )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
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


	//After all possible exceptions, write values to registers
	for ( i = 0; i < ( *Parser ).Request16.RegisterCount; i++ )
		MODBUSSlave.Registers[( *Parser ).Request16.FirstRegister + i] = modbusSwapEndian( ( *Parser ).Request16.Values[i] );

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *Builder ).Response16.Address = MODBUSSlave.Address;
	( *Builder ).Response16.Function = ( *Parser ).Request16.Function;
	( *Builder ).Response16.FirstRegister = modbusSwapEndian( ( *Parser ).Request16.FirstRegister );
	( *Builder ).Response16.RegisterCount = modbusSwapEndian( ( *Parser ).Request16.RegisterCount );

	//Calculate CRC
	( *Builder ).Response16.CRC = modbusCRC( ( *Builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
