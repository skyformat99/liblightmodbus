#include "slave.h"

 //Slave configuration
MODBUSSlaveStatus MODBUSSlave;

void MODBUSBuildException( uint8_t Function, uint8_t ExceptionCode )
{
	//Generates modbus exception frame in allocated memory frame
	//Returns generated frame length

	//Allocate memory for union
	union MODBUSParser *Exception = (union MODBUSParser *) malloc( 5 );

	//Reallocate frame memory
	MODBUSSlave.Response.Frame = (uint8_t *) realloc( MODBUSSlave.Response.Frame, 5 );
	memset( MODBUSSlave.Response.Frame, 0, 5 );

	//Setup exception frame
	( *Exception ).Exception.Address = MODBUSSlave.Address;
	( *Exception ).Exception.Function = ( 1 << 7 ) | Function;
	( *Exception ).Exception.ExceptionCode = ExceptionCode;
	( *Exception ).Exception.CRC = MODBUSCRC16( ( *Exception ).Frame, 3 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlave.Response.Frame, ( *Exception ).Frame, 5 );

	//Set frame length - frame is ready
	MODBUSSlave.Response.Length = 5;

	//Free memory used for union
	free( Exception );
}

void MODBUSParseRequest( uint8_t *Frame, uint8_t FrameLength )
{
	//Parse and interpret given modbus frame on slave-side

	//Init parser union
	//This one is actually unsafe, so it's easy to create a segmentation fault, so be careful here
	//Allowable frame array size in union is 256, but here I'm only allocating amount of frame length
	//It is even worse, compiler won't warn you, when you are outside the range
	//It works, and it uses much less memory, so I guess a bit of risk is fine in this case
	//Also, user needs to free memory alocated for frame himself!

	uint8_t ParseError = 0;

	union MODBUSParser *Parser = (union MODBUSParser *) malloc( FrameLength );
	memcpy( ( *Parser ).Frame, Frame, FrameLength );

	//Reset response frame status
	MODBUSSlave.Response.Length = 0;

	//If frame is not broadcasted and address doesn't match skip parsing
	if ( ( *Parser ).Base.Address != MODBUSSlave.Address && ( *Parser ).Base.Address != 0 )
	{
		free( Parser );
		return;
	}

	switch ( ( *Parser ).Base.Function )
	{
		case 1: //Read multiple coils
			if ( MODBUS_SLAVE_COILS ) MODBUSParseRequest01( Parser );
			else ParseError = 1;
			break;

		case 2: //Read multiple discrete inputs
			if ( MODBUS_SLAVE_DISCRETE_INPUTS ) MODBUSParseRequest02( Parser );
			else ParseError = 1;
			break;

		case 3: //Read multiple holding registers
			if ( MODBUS_SLAVE_REGISTERS ) MODBUSParseRequest03( Parser );
			else ParseError = 1;
			break;

		case 4: //Read multiple holding registers
			if ( MODBUS_SLAVE_INPUT_REGISTERS ) MODBUSParseRequest04( Parser );
			else ParseError = 1;
			break;

		case 5: //Write single coil
			if ( MODBUS_SLAVE_COILS ) MODBUSParseRequest05( Parser );
			else ParseError = 1;
			break;

		case 6: //Write single holding register
			if ( MODBUS_SLAVE_REGISTERS ) MODBUSParseRequest06( Parser );
			else ParseError = 1;
			break;

		case 15: //Write multiple coils
			if ( MODBUS_SLAVE_COILS ) MODBUSParseRequest15( Parser );
			else ParseError = 1;
			break;

		case 16: //Write multiple holding registers
			if ( MODBUS_SLAVE_REGISTERS ) MODBUSParseRequest16( Parser );
			else ParseError = 1;
			break;

		default:
			ParseError = 1;
			break;
	}

	if ( ParseError )
		if ( ( *Parser ).Base.Address != 0 ) MODBUSBuildException( ( *Parser ).Base.Function, 0x01 );

	free( Parser );
}

void MODBUSSlaveInit( uint8_t Address )
{
	//Very basic init of slave side
	//User has to modify pointers etc. himself

	MODBUSSlave.Address = Address;

	//Reset response frame status
	MODBUSSlave.Response.Length = 0;
	MODBUSSlave.Response.Frame = (uint8_t *) malloc( 8 );
}
