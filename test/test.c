#include "test.h"

/*
This is really simple test suite, it covers ~95% of library code
*/

uint16_t Registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
uint8_t Coils[4] = { 0, 0, 0, 0 };
uint8_t DiscreteInputs[2] = { 255, 0 };
uint16_t InputRegisters[4] = { 1, 2, 3, 4 };
uint16_t TestValues[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAAFF, 0xBBFF };
uint16_t TestValues2[512];
uint8_t TestValues3[2] = { 0b11001100, 0x00 };

void TermRGB( unsigned char R, unsigned char G, unsigned char B )
{
    if ( R > 5u || G > 5u || B > 5u ) return;
    printf( "\033[38;5;%dm", 16 + B + G * 6 + R * 36 );
}

void Test( )
{
	uint8_t i = 0;
	uint8_t SlaveError, MasterError;

	//Clear registers
	memset( Registers, 0, 8 * 2 );
	memset( InputRegisters, 0, 4 * 2 );
	memset( Coils, 0, 4 );
	memset( DiscreteInputs, 0, 2 );

	//Start test
	//TermRGB( 4, 2, 0 );
	printf( "Test started...\n" );

	//Dump frame
	//TermRGB( 2, 4, 0 );
	printf( "Dump the frame:\n\t" );
	for ( i = 0; i < MODBUSMaster.Request.Length; i++ )
		printf( "%.2x%s", MODBUSMaster.Request.Frame[i], ( i == MODBUSMaster.Request.Length - 1 ) ? "\n" : "-" );

	//Dump slave registers
	//TermRGB( 2, 4, 2 );
	printf( "Dump registers:\n\t" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "0x%.2x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump input registers:\n\t" );
	for ( i = 0; i < MODBUSSlave.InputRegisterCount; i++ )
		printf( "0x%.2x%s", InputRegisters[i], ( i == MODBUSSlave.InputRegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump coils:\n\t" );
	for ( i = 0; i < MODBUSSlave.CoilCount; i++ )
		printf( "%d%s", MODBUSReadMaskBit( Coils, MODBUSSlave.CoilCount, i ), ( i == MODBUSSlave.CoilCount - 1 ) ? "\n" : ", " );

	printf( "Dump discrete inputs:\n\t" );
	for ( i = 0; i < MODBUSSlave.DiscreteInputCount; i++ )
		printf( "%d%s", MODBUSReadMaskBit( DiscreteInputs, MODBUSSlave.DiscreteInputCount, i ), ( i == MODBUSSlave.DiscreteInputCount - 1 ) ? "\n" : ", " );

	//Parse request
	printf( "Let slave parse frame...\n" );
	SlaveError = MODBUSParseRequest( MODBUSMaster.Request.Frame, MODBUSMaster.Request.Length );
	printf( "\tError - %d\n\r\tFinished - %d\n\r", SlaveError, MODBUSSlave.Finished );

	//Dump slave registers
	printf( "Dump registers:\n\t" );
	for ( i = 0; i < MODBUSSlave.RegisterCount; i++ )
		printf( "0x%.2x%s", Registers[i], ( i == MODBUSSlave.RegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump input registers:\n\t" );
	for ( i = 0; i < MODBUSSlave.InputRegisterCount; i++ )
		printf( "0x%.2x%s", InputRegisters[i], ( i == MODBUSSlave.InputRegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump coils:\n\t" );
	for ( i = 0; i < MODBUSSlave.CoilCount; i++ )
		printf( "%d%s", MODBUSReadMaskBit( Coils, MODBUSSlave.CoilCount, i ), ( i == MODBUSSlave.CoilCount - 1 ) ? "\n" : ", " );

	printf( "Dump discrete inputs:\n\t" );
	for ( i = 0; i < MODBUSSlave.DiscreteInputCount; i++ )
		printf( "%d%s", MODBUSReadMaskBit( DiscreteInputs, MODBUSSlave.DiscreteInputCount, i ), ( i == MODBUSSlave.DiscreteInputCount - 1 ) ? "\n" : ", " );

	//Dump response
	printf( "Dump response - length = %d:\n\t", MODBUSSlave.Response.Length );
	for ( i = 0; i < MODBUSSlave.Response.Length; i++ )
		printf( "%x%s", MODBUSSlave.Response.Frame[i], ( i == MODBUSSlave.Response.Length - 1 ) ? "\n" : ", " );

	//Process response
	printf( "Let master process response...\n" );
	MasterError = MODBUSParseResponse( MODBUSSlave.Response.Frame, MODBUSSlave.Response.Length, MODBUSMaster.Request.Frame, MODBUSMaster.Request.Length );

	//Dump parsed data
	printf( "\tError - %d\n\r\tFinished - %d\n\r", MasterError, MODBUSMaster.Finished );
	for ( i = 0; i < MODBUSMaster.DataLength; i++ )
	{
		printf( "\t - { addr: 0x%x, type: 0x%x, reg: 0x%x, val: 0x%x }\n\r", MODBUSMaster.Data[i].Address, MODBUSMaster.Data[i].DataType, MODBUSMaster.Data[i].Register, MODBUSMaster.Data[i].Value );
	}
	if ( MasterError == MODBUS_ERROR_EXCEPTION )
	{
		//TermRGB( 4, 1, 0 );
		printf( "\t - ex addr: 0x%x, fun: 0x%x, code: 0x%x\n\r", MODBUSMaster.Exception.Address, MODBUSMaster.Exception.Function, MODBUSMaster.Exception.Code );
	}

	TermRGB( 4, 0, 0 );
	printf( "----------------------------------------\n\x1b[0m" );
}

void MainTest( )
{
	//Request03 - ok
	printf( "\x1b[0m03 - correct request...\n" );
	MODBUSBuildRequest03( 0x20, 0x00, 0x08 );
	Test( );

	//Request03 - bad CRC
	printf( "\x1b[0m03 - bad CRC...\n" );
	MODBUSBuildRequest03( 0x20, 0x00, 0x08 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
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

	//Request06 - ok
	printf( "\x1b[0m06 - correct request...\n" );
	MODBUSBuildRequest06( 0x20, 0x06, 0xf6 );
	Test( );

	//Request06 - bad crc
	printf( "\x1b[0m06 - bad CRC...\n" );
	MODBUSBuildRequest06( 0x20, 0x06, 0xf6 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
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

	//Request16 - bad crc
	printf( "\x1b[0m16 - bad CRC...\n" );
	MODBUSBuildRequest16( 0x20, 0x00, 0x04, TestValues );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
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

	//Request02 - ok
	printf( "\x1b[0m02 - correct request...\n" );
	MODBUSBuildRequest02( 0x20, 0x00, 0x10 );
	Test( );

	//Request02 - bad crc
	printf( "\x1b[0m02 - bad CRC...\n" );
	MODBUSBuildRequest02( 0x20, 0x00, 0x10 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
	Test( );

	//Request02 - bad first discrete input
	printf( "\x1b[0m02 - bad first discrete input...\n" );
	MODBUSBuildRequest02( 0x20, 0xff, 0x10 );
	Test( );

	//Request02 - bad discrete input count
	printf( "\x1b[0m02 - bad discrete input count...\n" );
	MODBUSBuildRequest02( 0x20, 0x00, 0xff );
	Test( );

	//Request02 - bad register count and first discrete input
	printf( "\x1b[0m02 - bad register count and first discrete input...\n" );
	MODBUSBuildRequest02( 0x20, 0xffff, 0xffff );
	Test( );

	//Request02 - broadcast
	printf( "\x1b[0m02 - broadcast...\n" );
	MODBUSBuildRequest02( 0x00, 0x00, 0x10 );
	Test( );

	//Request02 - other slave address
	printf( "\x1b[0m02 - other address...\n" );
	MODBUSBuildRequest02( 0x10, 0x00, 0x10 );
	Test( );

	//Request01 - ok
	printf( "\x1b[0m01 - correct request...\n" );
	MODBUSBuildRequest01( 0x20, 0x00, 0x04 );
	Test( );

	//Request01 - bad CRC
	printf( "\x1b[0m01 - bad CRC...\n" );
	MODBUSBuildRequest01( 0x20, 0x00, 0x04 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
	Test( );

	//Request01 - bad first register
	printf( "\x1b[0m01 - bad first coil...\n" );
	MODBUSBuildRequest01( 0x20, 0xff, 0x04 );
	Test( );

	//Request01 - bad register count
	printf( "\x1b[0m01 - bad coil count...\n" );
	MODBUSBuildRequest01( 0x20, 0x00, 0xff );
	Test( );

	//Request01 - bad register count and first register
	printf( "\x1b[0m01 - bad coil count and first coil...\n" );
	MODBUSBuildRequest01( 0x20, 0xffff, 0xffff );
	Test( );

	//Request01 - broadcast
	printf( "\x1b[0m01 - broadcast...\n" );
	MODBUSBuildRequest01( 0x00, 0x00, 0x04 );
	Test( );

	//Request01 - other slave address
	printf( "\x1b[0m01 - other address...\n" );
	MODBUSBuildRequest01( 0x10, 0x00, 0x04 );
	Test( );

	//Request05 - ok
	printf( "\x1b[0m05 - correct request...\n" );
	MODBUSBuildRequest05( 0x20, 0x03, 0xff00 );
	Test( );

	//Request05 - bad CRC
	printf( "\x1b[0m05 - bad CRC...\n" );
	MODBUSBuildRequest05( 0x20, 0x03, 0xff00 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
	Test( );

	//Request05 - ok
	printf( "\x1b[0m05 - correct request, non 0xff00 number...\n" );
	MODBUSBuildRequest05( 0x20, 0x03, 1 );
	Test( );

	//Request05 - bad register
	printf( "\x1b[0m05 - bad coil...\n" );
	MODBUSBuildRequest05( 0x20, 0xf3, 0xff00 );
	Test( );

	//Request05 - broadcast
	printf( "\x1b[0m05 - broadcast...\n" );
	MODBUSBuildRequest05( 0x00, 0x03, 0xff00 );
	Test( );

	//Request05 - other slave address
	printf( "\x1b[0m05 - other address...\n" );
	MODBUSBuildRequest05( 0x10, 0x03, 0xff00 );
	Test( );

	//Request15 - ok
	printf( "\x1b[0m15 - correct request...\n" );
	MODBUSBuildRequest15( 0x20, 0x00, 0x04, TestValues3 );
	Test( );

	//Request15 - bad crc
	printf( "\x1b[0m15 - bad CRC...\n" );
	MODBUSBuildRequest15( 0x20, 0x00, 0x04, TestValues3 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
	Test( );

	//Request15 - bad start register
	printf( "\x1b[0m15 - bad first coil...\n" );
	MODBUSBuildRequest15( 0x20, 0xFF, 0x04, TestValues3 );
	Test( );

	//Request15 - bad register range
	printf( "\x1b[0m15 - bad coil range...\n" );
	MODBUSBuildRequest15( 0x20, 0x00, 0xFF, TestValues3 );
	Test( );

	//Request15 - bad register range 2
	printf( "\x1b[0m15 - bad coil range 2...\n" );
	MODBUSBuildRequest15( 0x20, 0x00, 0x20, TestValues3 );
	Test( );

	//Request15 - confusing register range
	printf( "\x1b[0m15 - confusing coil range...\n" );
	MODBUSBuildRequest15( 0x20, 0x00, 0x40, TestValues3 );
	Test( );

	//Request15 - confusing register range 2
	printf( "\x1b[0m15 - confusing coil range 2...\n" );
	MODBUSBuildRequest15( 0x20, 0x01, 0x40, TestValues3 );
	Test( );

	//Request15 - broadcast
	printf( "\x1b[0m15 - broadcast...\n" );
	MODBUSBuildRequest15( 0x00, 0x00, 0x04, TestValues3 );
	Test( );

	//Request15 - other slave address
	printf( "\x1b[0m15 - other address...\n" );
	MODBUSBuildRequest15( 0x10, 0x00, 0x04, TestValues3 );
	Test( );

	//Request04 - ok
	printf( "\x1b[0m04 - correct request...\n" );
	MODBUSBuildRequest04( 0x20, 0x00, 0x04 );
	Test( );

	//Request04 - bad crc
	printf( "\x1b[0m04 - bad CRC...\n" );
	MODBUSBuildRequest04( 0x20, 0x00, 0x04 );
	MODBUSMaster.Request.Frame[MODBUSMaster.Request.Length - 1]++;
	Test( );

	//Request04 - bad first register
	printf( "\x1b[0m04 - bad first register...\n" );
	MODBUSBuildRequest04( 0x20, 0x05, 0x04 );
	Test( );

	//Request04 - bad register count
	printf( "\x1b[0m04 - bad register count...\n" );
	MODBUSBuildRequest04( 0x20, 0x00, 0x05 );
	Test( );

	//Request04 - bad register count and first register
	printf( "\x1b[0m04 - bad register count and first register...\n" );
	MODBUSBuildRequest04( 0x20, 0x01, 0x05 );
	Test( );

	//Request04 - broadcast
	printf( "\x1b[0m04 - broadcast...\n" );
	MODBUSBuildRequest04( 0x00, 0x00, 0x04 );
	Test( );

	//Request04 - other slave address
	printf( "\x1b[0m04 - other address...\n" );
	MODBUSBuildRequest04( 0x10, 0x00, 0x04 );
	Test( );

	//WRITE PROTECTION TEST
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
	MODBUSSlave.Registers = Registers;
	MODBUSSlave.RegisterCount = 8;

	MODBUSSlave.Coils = Coils;
	MODBUSSlave.CoilCount = 32;

	MODBUSSlave.DiscreteInputs = DiscreteInputs;
	MODBUSSlave.DiscreteInputCount = 16;

	MODBUSSlave.InputRegisters = InputRegisters;
	MODBUSSlave.InputRegisterCount = 4;

	MODBUSSlaveInit( 32 );
	MODBUSMasterInit( );

	MainTest( );

	//Reset terminal colors
	printf( "\x1b[0m" );

	return 0;
}
