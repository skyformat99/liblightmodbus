#include "../../include/modlib/modlib.h"
#include "../../include/modlib/parser.h"
#include "../../include/modlib/slave/stypes.h"
#include "../../include/modlib/slave/sregisters.h"

//Use external slave configuration
extern MODBUSSlaveStatus MODBUSSlave;

uint8_t MODBUSBuildResponse03( union MODBUSParser *Parser )
{
	//Response for master request03
	//Frame length (with CRC) is: 5 + ( ( *Parser ).Request03.RegisterCount << 1 )
	//5 bytes of data and each register * 2b ( 1 + 1 + 1 + 2x + 2 )
	uint8_t FrameLength = 5 + ( ( *Parser ).Request03.RegisterCount << 1 );
	uint8_t i = 0;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return 0;

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
	( *Builder ).Response03.Address = MODBUSSlave.Address;
	( *Builder ).Response03.Function = ( *Parser ).Request03.Function;
	( *Builder ).Response03.BytesCount = ( *Parser ).Request03.RegisterCount << 1;

	//Copy registers to response frame
	for ( i = 0; i < ( *Parser ).Request03.RegisterCount; i++ )
		( *Builder ).Response03.Values[i] = MODBUSSwapEndian( MODBUSSlave.Registers[( *Parser ).Request03.FirstRegister + i] );

	//Calculate CRC
	( *Builder ).Response03.Values[( *Parser ).Request03.RegisterCount] = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Builder ).Frame, FrameLength );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;

	//Free union memory
	free( Builder );
	return 0;
}

uint8_t MODBUSBuildResponse06( union MODBUSParser *Parser )
{
	//Response for master request06
	//Frame length (with CRC) is: 8
	//8 bytes of data ( 1 + 1 + 2 + 2 + 2 )
	uint8_t FrameLength = 8;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return 0;

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
	( *Builder ).Response06.Address = MODBUSSlave.Address;
	( *Builder ).Response06.Function = ( *Parser ).Request06.Function;
	( *Builder ).Response06.Register = MODBUSSwapEndian( ( *Parser ).Request06.Register );
	( *Builder ).Response06.Value = MODBUSSwapEndian( MODBUSSlave.Registers[( *Parser ).Request06.Register] );

	//Calculate CRC
	( *Builder ).Response06.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Builder ).Frame, FrameLength );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;

	//Free union memory
	free( Builder );
	return 0;
}

uint8_t MODBUSBuildResponse16( union MODBUSParser *Parser )
{
	//Response for master request16
	//Frame length (with CRC) is: 8
	//8 bytes of data ( 1 + 1 + 2 + 2 + 2 )
	uint8_t FrameLength = 8;

	//Do not respond when frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return 0;

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
	( *Builder ).Response16.Address = MODBUSSlave.Address;
	( *Builder ).Response16.Function = ( *Parser ).Request16.Function;
	( *Builder ).Response16.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request16.FirstRegister );
	( *Builder ).Response16.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request16.RegisterCount );

	//Calculate CRC
	( *Builder ).Response16.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Builder ).Frame, FrameLength );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = FrameLength;

	//Free union memory
	free( Builder );
	return 0;
}

uint8_t MODBUSParseRequest03( union MODBUSParser *Parser )
{
	//Read multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request03.CRC ) return MODBUS_ERROR_CRC;

	//Ignore read request if frame is broadcasted
	if ( ( *Parser ).Base.Address == 0 ) return 0;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request03.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request03.FirstRegister );
	( *Parser ).Request03.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request03.RegisterCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request03.RegisterCount == 0 )
	{
		//Illegal data value error
		return MODBUSBuildException( 0x03, 0x03 );
	}

	if ( ( *Parser ).Request03.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		return MODBUSBuildException( 0x03, 0x02 );
	}

	if ( ( *Parser ).Request03.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *Parser ).Request03.FirstRegister + (uint32_t) ( *Parser ).Request03.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		return MODBUSBuildException( 0x03, 0x02 );
	}

	//Respond
	return MODBUSBuildResponse03( Parser );
}

uint8_t MODBUSParseRequest06( union MODBUSParser *Parser )
{
	//Write single holding register
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request06.CRC ) return MODBUS_ERROR_CRC;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request06.Register = MODBUSSwapEndian( ( *Parser ).Request06.Register );
	( *Parser ).Request06.Value = MODBUSSwapEndian( ( *Parser ).Request06.Value );

	//Check if register is in valid range
	if ( ( *Parser ).Request06.Register >= MODBUSSlave.RegisterCount )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x06, 0x02 );
		return 0;
	}

	//Check if register is allowed to be written
	if ( MODBUSReadMaskBit( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *Parser ).Request06.Register ) == 1 )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x06, 0x02 );
		return 0;
	}

	//Write register
	MODBUSSlave.Registers[( *Parser ).Request06.Register] = ( *Parser ).Request06.Value;

	//Respond
	return MODBUSBuildResponse06( Parser );
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
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request16.Values[( *Parser ).Request16.BytesCount >> 1] ) return MODBUS_ERROR_CRC;

	//Check if bytes or registers count isn't 0
	if ( ( *Parser ).Request16.BytesCount == 0 || ( *Parser ).Request16.RegisterCount == 0 )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x10, 0x03 );
		return 0;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request16.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request16.FirstRegister );
	( *Parser ).Request16.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request16.RegisterCount );

	//Check if bytes count *2 is equal to registers count
	if ( ( *Parser ).Request16.RegisterCount != ( ( *Parser ).Request16.BytesCount >> 1 ) )
	{
		//Illegal data value error
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x10, 0x03 );
		return 0;
	}

	//Check if registers are in valid range
	if ( ( *Parser ).Request16.RegisterCount > MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x10, 0x02 );
		return 0;
	}

	if ( ( *Parser ).Request16.FirstRegister >= MODBUSSlave.RegisterCount || (uint32_t) ( *Parser ).Request16.FirstRegister + (uint32_t) ( *Parser ).Request16.RegisterCount > (uint32_t) MODBUSSlave.RegisterCount )
	{
		//Illegal data address error
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x10, 0x02 );
		return 0;
	}

	//Check for write protection
	for ( i = 0; i < ( *Parser ).Request16.RegisterCount; i++ )
		MaskSum += (  MODBUSReadMaskBit( MODBUSSlave.RegisterMask, MODBUSSlave.RegisterMaskLength, ( *Parser ).Request16.FirstRegister + i ) == 1 ) ? 1 : 0;

	if ( MaskSum > 0 )
	{
		//Illegal data address exception
		if ( ( *Parser ).Base.Address != 0 ) return MODBUSBuildException( 0x10, 0x02 );
		return 0;
	}

	//Write values to registers
	for ( i = 0; i < ( *Parser ).Request16.RegisterCount; i++ )
		MODBUSSlave.Registers[( *Parser ).Request16.FirstRegister + i] = MODBUSSwapEndian( ( *Parser ).Request16.Values[i] );

	//Respond
	return MODBUSBuildResponse16( Parser );
}
