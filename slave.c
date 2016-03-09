#include "slave.h"
#include <stdio.h>

uint8_t MODBUSAddress; //Address of device
uint16_t *MODBUSRegisters; //Pointer to slave-side modbus registers
uint16_t MODBUSRegisterCount; //Count of slave-side modbus registers

MODBUSResponseStatus MODBUSSlaveResponse; //Name contains "slave", beacuse on master side "MODBUSResponse" is data received


void MODBUSException( uint8_t Function, uint8_t ExceptionCode )
{
	//Generates modbus exception frame in allocated memory frame
	//Returns generated frame length

	//Allocate memory for union
	union MODBUSException *Exception = malloc( 5 );
	MODBUSSlaveResponse.Frame = realloc( MODBUSSlaveResponse.Frame, 5 );
	memset( MODBUSSlaveResponse.Frame, 0, 5 );

	//Setup exception frame
	( *Exception ).Exception.Address = MODBUSAddress;
	( *Exception ).Exception.Function = ( 1 << 7 ) | Function;
	( *Exception ).Exception.ExceptionCode = ExceptionCode;
	( *Exception ).Exception.CRC = MODBUSCRC16( ( *Exception ).Frame, 3 );

	//Copy result from union to frame pointer
	memcpy( MODBUSSlaveResponse.Frame, ( *Exception ).Frame, 5 );

	//Set frame length - frame is ready
	MODBUSSlaveResponse.Length = 5;

	//Free memory used for union
	free( Exception );
}

void MODBUSRequest03( union MODBUSParser *Parser )
{
	//Read multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request03.CRC ) return;

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request03.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request03.FirstRegister );
	( *Parser ).Request03.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request03.RegisterCount );

	//Check if register is in valid range
	if ( ( *Parser ).Request03.FirstRegister >= MODBUSRegisterCount || ( *Parser ).Request03.FirstRegister + ( *Parser ).Request03.RegisterCount > MODBUSRegisterCount )
	{
		//Illegal data address exception
		MODBUSException( 0x03, 0x02 );
		return;
	}

	//FORMAT RESPONSE HERE
	printf( "OK\n" ); //*DEBUG*
}

void MODBUSRequest06( union MODBUSParser *Parser )
{
	//Write single holding register
	//Using data from union pointer

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request06.CRC ) return; //EXCEPTION (in future)

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request06.Register = MODBUSSwapEndian( ( *Parser ).Request06.Register );
	( *Parser ).Request06.Value = MODBUSSwapEndian( ( *Parser ).Request06.Value );

	//Check if register is in valid range
	if ( ( *Parser ).Request06.Register >= MODBUSRegisterCount )
	{
		//Illegal data address exception
		MODBUSException( 0x06, 0x02 );
		return;
	}

	//Write register
	MODBUSRegisters[( *Parser ).Request06.Register] = ( *Parser ).Request06.Value;

	//FORMAT RESPONSE HERE
	printf( "OK\n" ); //*DEBUG*
}

void MODBUSRequest16( union MODBUSParser *Parser )
{
	//Write multiple holding registers
	//Using data from union pointer

	//Update frame length
	uint8_t i = 0;
	uint8_t FrameLength = 9 + ( *Parser ).Request16.BytesCount;

	//Check frame CRC
	//Shifting is used instead of dividing for optimisation on smaller devices (AVR)
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Request16.Values[( *Parser ).Request16.BytesCount >> 1] ) return; //EXCEPTION (in future)

	//Check if bytes or registers count isn't 0
	if ( ( *Parser ).Request16.BytesCount == 0 || ( *Parser ).Request16.RegisterCount == 0 )
	{
		//Illegal data value error
		MODBUSException( 0x10, 0x03 );
		return;
	}

	//Swap endianness of longer members (but not CRC)
	( *Parser ).Request16.FirstRegister = MODBUSSwapEndian( ( *Parser ).Request16.FirstRegister );
	( *Parser ).Request16.RegisterCount = MODBUSSwapEndian( ( *Parser ).Request16.RegisterCount );

	//Check if bytes count *2 is equal to registers count
	if ( ( *Parser ).Request16.RegisterCount != ( ( *Parser ).Request16.BytesCount >> 1 ) )
	{
		//Illegal data value error
		MODBUSException( 0x10, 0x03 );
		return;
	}

	//Check if registers are in valid range
	if ( ( *Parser ).Request16.FirstRegister >= MODBUSRegisterCount || ( *Parser ).Request16.FirstRegister + ( *Parser ).Request16.RegisterCount > MODBUSRegisterCount )
	{
		//Illegal data address error
		MODBUSException( 0x10, 0x02 );
		return; //EXCEPTION (in future)
	}

	//Write values to registers
	for ( i = 0; i < ( *Parser ).Request16.RegisterCount; i++ )
		MODBUSRegisters[( *Parser ).Request16.FirstRegister + i] = ( *Parser ).Request16.Values[i];

	//FORMAT RESPONSE HERE
	printf( "OK\n" ); //*DEBUG*
}

void MODBUSParseRequest( uint8_t *Frame, uint8_t FrameLength )
{
	//Parse and interpret given modbus frame on slave-side

	//Init parser union
	//This one is actually unsafe, so it's easy to create a segmentation fault, so be careful here
	//Allowable frame array size in union is 256, but here I'm only allocating amount of frame length
	//It is even worse, compiler won't warn you, when you are outside the range
	//It works, and it uses much less memory, so I guess a bit of risk is fine in this case
	//If something goes wrong, this can be changed back
	//Also, user needs to free memory alocated for frame himself!
	union MODBUSParser *Parser;
	Parser = malloc( FrameLength );
	memcpy( ( *Parser ).Frame, Frame, FrameLength );

	//Reset response frame status
	MODBUSSlaveResponse.Length = 0;


	//If frame is not broadcasted and address doesn't match skip parsing
	if ( ( *Parser ).Base.Address != MODBUSAddress && ( *Parser ).Base.Address != 0 )
	{
		free( Parser );
		return;
	}

	switch( ( *Parser ).Base.Function )
	{
		case 3: //Read multiple holding registers
			MODBUSRequest03( Parser );
			break;

		case 6: //Write single holding register
			MODBUSRequest06( Parser );
			break;

		case 16: //Write multiple holding registers
			MODBUSRequest16( Parser );
			break;

		default:
			//Illegal function exception
			MODBUSException( ( *Parser ).Base.Function, 0x01 );
			break;
	}

	free( Parser );
}

void MODBUSSlaveInit( uint8_t Address, uint16_t *Registers, uint16_t RegisterCount )
{
	//Very basic init of slave side
	MODBUSAddress = Address;
	MODBUSRegisters = Registers;
	MODBUSRegisterCount = RegisterCount;

	//Reset response frame status
	MODBUSSlaveResponse.Length = 0;
	MODBUSSlaveResponse.Frame = malloc( 8 );
}
