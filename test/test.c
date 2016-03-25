#include "test.h"

uint16_t Registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
//uint8_t Frame[256] = { 0x20, 0x10, 0x00, 0x03, 0x00, 0x02, 0x04, 0xFA, 0xFB, 0xEA, 0xEB }; //R16
//uint8_t FrameLength = 11; //R16
//uint8_t Frame[256] = { 0x20, 0x03, 0x00, 0x02, 0x00, 0x04 }; //R03
//uint8_t FrameLength = 6; //R03
uint8_t Frame[256] = { 0x20, 0x06, 0x00, 0x02, 0x00, 0x04 }; //R06
uint8_t FrameLength = 6; //R06

void TermRGB( unsigned char R, unsigned char G, unsigned char B )
{
    if ( R > 5u || G > 5u || B > 5u ) return;
    printf( "\033[38;5;%dm", 16 + B + G * 6 + R * 36 );
}

int main( )
{
	unsigned char i = 0;

	TermRGB( 4, 1, 0 );
	printf( "\nInit...\n" );

	//Init slave-side
	MODBUSSlave.Registers = Registers;
	MODBUSSlave.RegisterCount = 8;
	MODBUSSlaveInit( 32, MODBUSSlave.Registers, MODBUSSlave.RegisterCount );
	MODBUSMasterInit( );

	TermRGB( 4, 2, 0 );
	printf( "Test started...\n" );

	TermRGB( 3, 3, 0 );
	printf( "Calculate CRC of given frame... %x\n", MODBUSCRC16( Frame, FrameLength ) );
	Frame[FrameLength] = MODBUSCRC16( Frame, FrameLength ) & 0xFF;
	Frame[FrameLength + 1] = ( MODBUSCRC16( Frame, FrameLength ) & 0xFF00 ) >> 8;

	TermRGB( 2, 4, 0 );
	printf( "Dump the frame:\n" );
	for ( i = 0; i < FrameLength + 2; i++ )
		printf( "%x%s", Frame[i], ( i == FrameLength + 1 ) ? "\n" : ", " );

	TermRGB( 2, 4, 2 );
	printf( "Dump registers:\n" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "%x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

	TermRGB( 1, 4, 3 );
	printf( "Let slave parse frame...\n" );

	TermRGB( 0, 3, 4 );
	MODBUSParseRequest( Frame, FrameLength + 2 );

	TermRGB( 0, 2, 4 );
	printf( "Dump registers:\n" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "%x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

	TermRGB( 0, 1, 4 );
	printf( "Dump response - length = %d:\n", MODBUSSlave.Response.Length );
	for ( i = 0; i < MODBUSSlave.Response.Length; i++ )
		printf( "%x%s", MODBUSSlave.Response.Frame[i], ( i == MODBUSSlave.Response.Length - 1 ) ? "\n" : ", " );

	TermRGB( 0, 2, 4 );
	printf( "Let master process response...\n" );

	MODBUSParseResponse( MODBUSSlave.Response.Frame, MODBUSSlave.Response.Length, Frame, FrameLength + 2 );

	TermRGB( 0, 3, 4 );
	printf( "Error - %d\n\rFinished - %d\n\r", MODBUSMaster.Error, MODBUSMaster.Finished );
	for ( i = 0; i < MODBUSMaster.DataLength; i++ )
	{
		printf( "\t - { addr: 0x%x, type: 0x%x, reg: 0x%x, val: 0x%x }\n\r", MODBUSMaster.Data[i].Address, MODBUSMaster.Data[i].DataType, MODBUSMaster.Data[i].Register, MODBUSMaster.Data[i].Value );
	}
	if ( MODBUSMaster.Error )
	{
		TermRGB( 4, 1, 0 );
		printf( "\t - ex addr: 0x%x, fun: 0x%x, code: 0x%x\n\r", MODBUSMaster.Exception.Address, MODBUSMaster.Exception.Function, MODBUSMaster.Exception.Code );
	}

	return 0;
}
