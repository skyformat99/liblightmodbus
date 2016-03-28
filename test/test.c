#include "test.h"

/*
This is really simple test suite, it covers ~95% of library code
*/

uint16_t Registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
uint16_t TestValues[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAAFF, 0xBBFF };
uint16_t TestValues2[512];

void TermRGB( unsigned char R, unsigned char G, unsigned char B )
{
    if ( R > 5u || G > 5u || B > 5u ) return;
    printf( "\033[38;5;%dm", 16 + B + G * 6 + R * 36 );
}

void Test( )
{
	uint8_t i = 0;

	//Clear registers
	memset( Registers, 0, 8 * 2 );

	//Start test
	TermRGB( 4, 2, 0 );
	printf( "Test started...\n" );

	//Dump frame
	TermRGB( 2, 4, 0 );
	printf( "Dump the frame:\n" );
	for ( i = 0; i < MODBUSMaster.Request.Length; i++ )
		printf( "%.2x%s", MODBUSMaster.Request.Frame[i], ( i == MODBUSMaster.Request.Length - 1 ) ? "\n" : "-" );

	//Dump slave registers
	TermRGB( 2, 4, 2 );
	printf( "Dump registers:\n" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "0x%.2x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

		//Parse request
	TermRGB( 1, 4, 3 );
	printf( "Let slave parse frame...\n" );
	MODBUSParseRequest( MODBUSMaster.Request.Frame, MODBUSMaster.Request.Length );

	//Dump slave registers
	TermRGB( 0, 2, 4 );
	printf( "Dump registers:\n" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "0x%.2x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

		//Dump response
	TermRGB( 0, 1, 4 );
	printf( "Dump response - length = %d:\n", MODBUSSlave.Response.Length );
	for ( i = 0; i < MODBUSSlave.Response.Length; i++ )
		printf( "%x%s", MODBUSSlave.Response.Frame[i], ( i == MODBUSSlave.Response.Length - 1 ) ? "\n" : ", " );

		//Process response
	TermRGB( 0, 2, 4 );
	printf( "Let master process response...\n" );
	MODBUSParseResponse( MODBUSSlave.Response.Frame, MODBUSSlave.Response.Length, MODBUSMaster.Request.Frame, MODBUSMaster.Request.Length );

	//Dump parsed data
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
  printf( "\x1b[0m----------------------------------------\n" );
}

void MainTest( )
{
	///*
	//Request03 - ok
	printf( "\x1b[0m03 - correct request...\n" );
	MODBUSBuildRequest03( 0x20, 0x00, 0x08 );
	Test( );

	//Request03 - bad first register
	printf( "\x1b[0m03 - bad first register...\n" );
	MODBUSBuildRequest03( 0x20, 0xff, 0x08 );
	Test( );

	//Request03 - bad register count
	printf( "\x1b[0m03 - bad register count...\n" );
	MODBUSBuildRequest03( 0x20, 0x00, 0xff );
	Test( );

	//Request03 - bad register count and first register
	printf( "\x1b[0m03 - bad register count and first register...\n" );
	MODBUSBuildRequest03( 0x20, 0xffff, 0xffff );
	Test( );

	//Request03 - broadcast
	printf( "\x1b[0m03 - broadcast...\n" );
	MODBUSBuildRequest03( 0x00, 0x00, 0x08 );
	Test( );

	//Request03 - other slave address
	printf( "\x1b[0m03 - other address...\n" );
	MODBUSBuildRequest03( 0x10, 0x00, 0x08 );
	Test( );
	//*/
	///*
	//Request06 - ok
	printf( "\x1b[0m06 - correct request...\n" );
	MODBUSBuildRequest06( 0x20, 0x06, 0xf6 );
	Test( );

	//Request06 - bad register
	printf( "\x1b[0m06 - bad register...\n" );
	MODBUSBuildRequest06( 0x20, 0xf6, 0xf6 );
	Test( );

	//Request06 - broadcast
	printf( "\x1b[0m06 - broadcast...\n" );
	MODBUSBuildRequest06( 0x00, 0x06, 0xf6 );
	Test( );

	//Request06 - other slave address
	printf( "\x1b[0m06 - other address...\n" );
	MODBUSBuildRequest06( 0x10, 0x06, 0xf6 );
	Test( );

	//Request16 - ok
	printf( "\x1b[0m16 - correct request...\n" );
	MODBUSBuildRequest16( 0x20, 0x00, 0x04, TestValues );
	Test( );

	//Request16 - bad start register
	printf( "\x1b[0m16 - bad first register...\n" );
	MODBUSBuildRequest16( 0x20, 0xFF, 0x04, TestValues );
	Test( );

	//Request16 - bad register range
	printf( "\x1b[0m16 - bad register range...\n" );
	MODBUSBuildRequest16( 0x20, 0x00, 0xFF, TestValues2 );
	Test( );

	//Request16 - bad register range 2
	printf( "\x1b[0m16 - bad register range 2...\n" );
	MODBUSBuildRequest16( 0x20, 0x00, 0x20, TestValues2 );
	Test( );

	//Request16 - confusing register range
	printf( "\x1b[0m16 - confusing register range...\n" );
	MODBUSBuildRequest16( 0x20, 0x00, 0x08, TestValues );
	Test( );

	//Request16 - confusing register range 2
	printf( "\x1b[0m16 - confusing register range 2...\n" );
	MODBUSBuildRequest16( 0x20, 0x01, 0x08, TestValues );
	Test( );

	//Request16 - broadcast
	printf( "\x1b[0m16 - broadcast...\n" );
	MODBUSBuildRequest16( 0x00, 0x00, 0x04, TestValues );
	Test( );

	//Request16 - other slave address
	printf( "\x1b[0m16 - other address...\n" );
	MODBUSBuildRequest16( 0x10, 0x00, 0x04, TestValues );
	Test( );
	//*/
}

void WriteProtectionTest( )
{
	uint8_t Mask[1] = { 0 };
	MODBUSSlave.RegisterMask = Mask;
	MODBUSSlave.RegisterMaskLength = 1;

	MODBUSWriteMaskBit( Mask, 1, 2, 1 );

	MODBUSBuildRequest06( 0x20, 2, 16 );
	Test( );
	MODBUSBuildRequest06( 0x20, 0, 16 );
	Test( );

	MODBUSBuildRequest16( 0x20, 0, 4, TestValues2 );
	Test( );
	MODBUSBuildRequest16( 0x20, 0, 2, TestValues2 );
	Test( );

	printf( "Bitval: %d\r\n", MODBUSReadMaskBit( Mask, 1, 0 ) );
	printf( "Bitval: %d\r\n", MODBUSReadMaskBit( Mask, 1, 1 ) );
	printf( "Bitval: %d\r\n", MODBUSReadMaskBit( Mask, 1, 2 ) );
	printf( "Bitval: %d\r\n", MODBUSReadMaskBit( Mask, 1, 3 ) );
	printf( "Bitval: %d\r\n", MODBUSReadMaskBit( Mask, 1, 4 ) );

	MODBUSSlave.RegisterMaskLength = 0;
}

int main( )
{
	TermRGB( 4, 1, 0 );
	printf( "\nInit...\n" );
	memset( TestValues2, 0xAA, 1024 );

	//Init slave and master
	MODBUSSlaveInit( 32, Registers, 8 );
	MODBUSMasterInit( );

	MainTest( );
	WriteProtectionTest( );

	//Reset terminal colors
	printf( "\x1b[0m" );

	return 0;
}
