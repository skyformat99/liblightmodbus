#include "../include/lightmodbus/slave.h"

 //Slave configuration
MODBUSSlaveStatus MODBUSSlave;

uint8_t MODBUSBuildException( uint8_t Function, uint8_t ExceptionCode )
{
	//Generates modbus exception frame in allocated memory frame
	//Returns generated frame length

	//Reallocate frame memory
	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, 5 );
	if ( MODBUSSlave.Response.Frame == NULL )
	{
		free( MODBUSSlave.Response.Frame );
		return MODBUS_ERROR_ALLOC;
	}
	memset( MODBUSSlave.Response.Frame, 0, 5 );
	union MODBUSParser *Exception = (union MODBUSParser *) MODBUSSlave.Response.Frame;

	//Setup exception frame
	( *Exception ).Exception.Address = MODBUSSlave.Address;
	( *Exception ).Exception.Function = ( 1 << 7 ) | Function;
	( *Exception ).Exception.ExceptionCode = ExceptionCode;
	( *Exception ).Exception.CRC = MODBUSCRC16( ( *Exception ).Frame, 3 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = 5;
	MODBUSSlave.Finished = 1;

	return 0;
}

uint8_t MODBUSParseRequest( uint8_t *Frame, uint8_t FrameLength )
{
	//Parse and interpret given modbus frame on slave-side

	//Init parser union
	//This one is actually unsafe, so it's easy to create a segmentation fault, so be careful here
	//Allowed frame array size in union is 256, but here I'm only allocating amount of frame length
	//It is even worse, compiler won't warn you, when you are outside the range
	//It works, and it uses much less memory, so I guess a bit of risk is fine in this case
	//Also, user needs to free memory alocated for frame himself!

	//Note: CRC is not checked here, just because if there was some junk at the end of correct frame (wrong length) it would be ommited
	//In fact, user should care about things like that, and It would lower memory usage, so in future CRC can be verified right here

	uint8_t Error = 0;

	//Reset response frame status
	MODBUSSlave.Response.Length = 0;
	MODBUSSlave.Finished = 0;

	//If user tries to parse an empty frame return error (to avoid problems with memory allocation)
	if ( FrameLength == 0 ) return MODBUS_ERROR_OTHER;

	//This part right there, below should be optimized, but currently I'm not 100% sure, that parsing doesn't malform given frame
	//In this case it's just much easier to allocate new frame
	union MODBUSParser *Parser = (union MODBUSParser *) malloc( FrameLength );
	if ( Parser == NULL )
	{
		free( Parser );
		return MODBUS_ERROR_ALLOC;
	}

	memcpy( ( *Parser ).Frame, Frame, FrameLength );

	//If frame is not broadcasted and address doesn't match skip parsing
	if ( ( *Parser ).Base.Address != MODBUSSlave.Address && ( *Parser ).Base.Address != 0 )
	{
		free( Parser );
		MODBUSSlave.Finished = 1;
		return 0;
	}

	switch ( ( *Parser ).Base.Function )
	{
		case 1: //Read multiple coils
			if ( LIGHTMODBUS_SLAVE_COILS ) Error = MODBUSParseRequest01( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 2: //Read multiple discrete inputs
			if ( LIGHTMODBUS_SLAVE_DISCRETE_INPUTS ) Error = MODBUSParseRequest02( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 3: //Read multiple holding registers
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) Error = MODBUSParseRequest03( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 4: //Read multiple input registers
			if ( LIGHTMODBUS_SLAVE_INPUT_REGISTERS ) Error = MODBUSParseRequest04( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 5: //Write single coil
			if ( LIGHTMODBUS_SLAVE_COILS ) Error = MODBUSParseRequest05( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 6: //Write single holding register
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) Error = MODBUSParseRequest06( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 15: //Write multiple coils
			if ( LIGHTMODBUS_SLAVE_COILS ) Error = MODBUSParseRequest15( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		case 16: //Write multiple holding registers
			if ( LIGHTMODBUS_SLAVE_REGISTERS ) Error = MODBUSParseRequest16( Parser );
			else Error = MODBUS_ERROR_PARSE;
			break;

		default:
			Error = MODBUS_ERROR_PARSE;
			break;
	}

	if ( Error == MODBUS_ERROR_PARSE )
		if ( ( *Parser ).Base.Address != 0 ) Error = MODBUSBuildException( ( *Parser ).Base.Function, 0x01 );

	free( Parser );

	return Error;
}

uint8_t MODBUSSlaveInit( uint8_t Address )
{
	//Very basic init of slave side
	//User has to modify pointers etc. himself

	MODBUSSlave.Address = Address;

	//Reset response frame status
	MODBUSSlave.Finished = 0;
	MODBUSSlave.Response.Length = 0;
	MODBUSSlave.Response.Frame = (uint8_t *) malloc( 8 );

	return ( ( MODBUSSlave.Response.Frame == NULL ) * MODBUS_ERROR_ALLOC ) | ( ( MODBUSSlave.Address == 0 ) * MODBUS_ERROR_OTHER );
}
