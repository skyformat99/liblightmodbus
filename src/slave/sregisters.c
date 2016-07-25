#include "../../include/lightmodbus/core.h"
#include "../../include/lightmodbus/parser.h"
#include "../../include/lightmodbus/slave/stypes.h"
#include "../../include/lightmodbus/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus_t MODBUSSlave;

uint8_t modbusParseRequest03( union MODBUSParser *parser )
{
	//Read multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;
	uint8_t i = 0;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Request03.CRC )
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
	( *parser ).Request03.FirstRegister = modbusSwapEndian( ( *parser ).Request03.FirstRegister );
	( *parser ).Request03.RegisterCount = modbusSwapEndian( ( *parser ).Request03.RegisterCount );

	//Check if register is in valid range
	if ( ( *parser ).Request03.RegisterCount == 0 )
	{
		//Illegal data value error
		return modbusBuildException( 0x03, 0x03 );
	}

	if ( ( *parser ).Request03.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		return modbusBuildException( 0x03, 0x02 );
	}

	if ( ( *parser ).Request03.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *parser ).Request03.FirstRegister + (uint32_t) ( *parser ).Request03.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		return modbusBuildException( 0x03, 0x02 );
	}

	//Respond
	FrameLength = 5 + ( ( *parser ).Request03.RegisterCount << 1 );

	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, FrameLength ); //Reallocate response frame memory to needed memory
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, FrameLength ); //Empty response frame
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//Set up basic response data
	( *builder ).Response03.Address = MODBUSSlave.Address;
	( *builder ).Response03.Function = ( *parser ).Request03.Function;
	( *builder ).Response03.BytesCount = ( *parser ).Request03.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *parser ).Request03.RegisterCount; i++ )
		( *builder ).Response03.Values[i] = modbusSwapEndian( MODBUSSlave.Registers[( *parser ).Request03.FirstRegister + i] );

	//Calculate CRC
	( *builder ).Response03.Values[( *parser ).Request03.RegisterCount] = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t modbusParseRequest06( union MODBUSParser *parser )
{
	//Write single holding register
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Request06.CRC )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Swap endianness of longer members (but not CRC)
	( *parser ).Request06.Register = modbusSwapEndian( ( *parser ).Request06.Register );
	( *parser ).Request06.Value = modbusSwapEndian( ( *parser ).Request06.Value );

	//Check if register is in valid range
	if ( ( *parser ).Request06.Register >= MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x06, 0x02 );
		return 0;
	}

	//Check if register is allowed to be written
	if ( modbusMaskRead( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *parser ).Request06.Register ) == 1 )
	{
		//Illegal data address exception
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x06, 0x02 );
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//After all possible exceptions, write register
	MODBUSSlave.Registers[( *parser ).Request06.Register] = ( *parser ).Request06.Value;

	//Do not respond when frame is broadcasted
	if ( ( *parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *builder ).Response06.Address = MODBUSSlave.Address;
	( *builder ).Response06.Function = ( *parser ).Request06.Function;
	( *builder ).Response06.Register = modbusSwapEndian( ( *parser ).Request06.Register );
	( *builder ).Response06.Value = modbusSwapEndian( MODBUSSlave.Registers[( *parser ).Request06.Register] );

	//Calculate CRC
	( *builder ).Response06.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t modbusParseRequest16( union MODBUSParser *parser )
{
	//Write multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t i = 0;
	uint8_t MaskSum = 0;
	uint8_t FrameLength = 9 + ( *parser ).Request16.BytesCount;

	//Check frame CRC
	//Shifting is used instead of dividing for optimisation on smaller devices (AVR)
	if ( modbusCRC( ( *parser ).Frame, FrameLength - 2 ) != ( *parser ).Request16.Values[( *parser ).Request16.BytesCount >> 1] )
	{
		MODBUSSlave.Finished = 1;
		return MODBUS_ERROR_CRC;
	}

	//Check if bytes or registers count isn't 0
	if ( ( *parser ).Request16.BytesCount == 0 || ( *parser ).Request16.RegisterCount == 0 )
	{
		//Illegal data value error
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x03 );
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *parser ).Request16.FirstRegister = modbusSwapEndian( ( *parser ).Request16.FirstRegister );
	( *parser ).Request16.RegisterCount = modbusSwapEndian( ( *parser ).Request16.RegisterCount );

	//Check if bytes count *2 is equal to registers count
	if ( ( *parser ).Request16.RegisterCount != ( ( *parser ).Request16.BytesCount >> 1 ) )
	{
		//Illegal data value error
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x03 );
		return 0;
	}

	//Check if registers are in valid range
	if ( ( *parser ).Request16.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
		return 0;
	}

	if ( ( *parser ).Request16.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *parser ).Request16.FirstRegister + (uint32_t) ( *parser ).Request16.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
		return 0;
	}

	//Check for write protection
	for ( i = 0; i < ( *parser ).Request16.RegisterCount; i++ )
		MaskSum += (  modbusMaskRead( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *parser ).Request16.FirstRegister + i ) == 1 ) ? 1 : 0;

	if ( MaskSum > 0 )
	{
		//Illegal data address exception
		if ( ( *parser ).Base.Address != 0 ) return modbusBuildException( 0x10, 0x02 );
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
	union MODBUSParser *builder = (union MODBUSParser *) MODBUSSlave.Response.Frame;


	//After all possible exceptions, write values to registers
	for ( i = 0; i < ( *parser ).Request16.RegisterCount; i++ )
		MODBUSSlave.Registers[( *parser ).Request16.FirstRegister + i] = modbusSwapEndian( ( *parser ).Request16.Values[i] );

	//Do not respond when frame is broadcasted
	if ( ( *parser ).Base.Address == 0 )
	{
		MODBUSSlave.Finished = 1;
		return 0;
	}

	//Set up basic response data
	( *builder ).Response16.Address = MODBUSSlave.Address;
	( *builder ).Response16.Function = ( *parser ).Request16.Function;
	( *builder ).Response16.FirstRegister = modbusSwapEndian( ( *parser ).Request16.FirstRegister );
	( *builder ).Response16.RegisterCount = modbusSwapEndian( ( *parser ).Request16.RegisterCount );

	//Calculate CRC
	( *builder ).Response16.CRC = modbusCRC( ( *builder ).Frame, FrameLength - 2 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;
	MODBUSSlave.Finished = 1;

	return 0;
}
