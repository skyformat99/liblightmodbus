#include "test.h"

/*
This is really simple test suite, it covers ~50% of library code
In future, when request-creating functions are implemented, this will test much more of modlib code
*/

uint16_t Registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

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

int main( )
{
	TermRGB( 4, 1, 0 );
	printf( "\nInit...\n" );

	//Init slave and master
	MODBUSSlaveInit( 32, Registers, 8 );
	MODBUSMasterInit( );

  //Request06 - ok
  printf( "\x1b[0mCorrect request...\n" );
  MODBUSBuildRequest06( 0x20, 0x06, 0xf6 );
  Test( );

  //Request06 - bad register
  printf( "\x1b[0mBad register...\n" );
  MODBUSBuildRequest06( 0x20, 0xf6, 0xf6 );
  Test( );

  //Request06 - broadcast
  printf( "\x1b[0mBroadcast...\n" );
  MODBUSBuildRequest06( 0x00, 0x06, 0xf6 );
  Test( );

  //Request06 - other slave address
  printf( "\x1b[0mOther address...\n" );
  MODBUSBuildRequest06( 0x10, 0x06, 0xf6 );
  Test( );

	//Reset terminal colors
	printf( "\x1b[0m" );

	return 0;
}
