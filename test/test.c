#include "test.h"

/*
This is really simple test suite, it covers ~95% of library code
*/

ModbusMasterStatus mstatus;
ModbusSlaveStatus sstatus;
uint16_t registers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
uint8_t coils[4] = { 0, 0, 0, 0 };
uint8_t discreteInputs[2] = { 255, 0 };
uint16_t inputRegisters[4] = { 1, 2, 3, 4 };
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
	memset( registers, 0, 8 * 2 );
	memset( inputRegisters, 0, 4 * 2 );
	memset( coils, 0, 4 );
	memset( discreteInputs, 0, 2 );

	//Start test
	//TermRGB( 4, 2, 0 );
	printf( "Test started...\n" );

	//Dump frame
	//TermRGB( 2, 4, 0 );
	printf( "Dump the frame:\n\t" );
	for ( i = 0; i < mstatus.request.length; i++ )
		printf( "%.2x%s", mstatus.request.frame[i], ( i == mstatus.request.length - 1 ) ? "\n" : "-" );

	//Dump slave registers
	//TermRGB( 2, 4, 2 );
	printf( "Dump registers:\n\t" );
	for ( i = 0; i < sstatus.registerCount; i++ )
		printf( "0x%.2x%s", registers[i], ( i == sstatus.registerCount - 1 ) ? "\n" : ", " );

	printf( "Dump input registers:\n\t" );
	for ( i = 0; i < sstatus.inputRegisterCount; i++ )
		printf( "0x%.2x%s", inputRegisters[i], ( i == sstatus.inputRegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump coils:\n\t" );
	for ( i = 0; i < sstatus.coilCount; i++ )
		printf( "%d%s", modbusMaskRead( coils, sstatus.coilCount, i ), ( i == sstatus.coilCount - 1 ) ? "\n" : ", " );

	printf( "Dump discrete inputs:\n\t" );
	for ( i = 0; i < sstatus.discreteInputCount; i++ )
		printf( "%d%s", modbusMaskRead( discreteInputs, sstatus.discreteInputCount, i ), ( i == sstatus.discreteInputCount - 1 ) ? "\n" : ", " );

	//Parse request
	printf( "Let slave parse frame...\n" );
	SlaveError = modbusParseRequest( &sstatus, mstatus.request.frame, mstatus.request.length );
	printf( "\tError - %d\n\tFinished - %d\n", SlaveError, sstatus.finished );

	//Dump slave registers
	printf( "Dump registers:\n\t" );
	for ( i = 0; i < sstatus.registerCount; i++ )
		printf( "0x%.2x%s", registers[i], ( i == sstatus.registerCount - 1 ) ? "\n" : ", " );

	printf( "Dump input registers:\n\t" );
	for ( i = 0; i < sstatus.inputRegisterCount; i++ )
		printf( "0x%.2x%s", inputRegisters[i], ( i == sstatus.inputRegisterCount - 1 ) ? "\n" : ", " );

	printf( "Dump coils:\n\t" );
	for ( i = 0; i < sstatus.coilCount; i++ )
		printf( "%d%s", modbusMaskRead( coils, sstatus.coilCount, i ), ( i == sstatus.coilCount - 1 ) ? "\n" : ", " );

	printf( "Dump discrete inputs:\n\t" );
	for ( i = 0; i < sstatus.discreteInputCount; i++ )
		printf( "%d%s", modbusMaskRead( discreteInputs, sstatus.discreteInputCount, i ), ( i == sstatus.discreteInputCount - 1 ) ? "\n" : ", " );

	//Dump response
	printf( "Dump response - length = %d:\n\t", sstatus.response.length );
	for ( i = 0; i < sstatus.response.length; i++ )
		printf( "%x%s", sstatus.response.frame[i], ( i == sstatus.response.length - 1 ) ? "\n" : ", " );

	//Process response
	printf( "Let master process response...\n" );
	MasterError = modbusParseResponse( &mstatus, sstatus.response.frame, sstatus.response.length, mstatus.request.frame, mstatus.request.length );

	//Dump parsed data
	printf( "\tError - %d\n\tFinished - %d\n", MasterError, mstatus.finished );
	for ( i = 0; i < mstatus.dataLength; i++ )
	{
		printf( "\t - { addr: 0x%x, type: 0x%x, reg: 0x%x, val: 0x%x }\n", mstatus.data[i].address, mstatus.data[i].dataType, mstatus.data[i].reg, mstatus.data[i].value );
	}
	if ( MasterError == MODBUS_ERROR_EXCEPTION )
	{
		//TermRGB( 4, 1, 0 );
		printf( "\t - ex addr: 0x%x, fun: 0x%x, code: 0x%x\n\r", mstatus.exception.address, mstatus.exception.function, mstatus.exception.code );
	}

	printf( "----------------------------------------\n\n" );
}

void MainTest( )
{
	//request03 - ok
	printf( "\t\t03 - correct request...\n" );
	modbusBuildRequest03( &mstatus, 0x20, 0x00, 0x08 );
	Test( );

	//request03 - bad CRC
	printf( "\t\t03 - bad CRC...\n" );
	modbusBuildRequest03( &mstatus, 0x20, 0x00, 0x08 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request03 - bad first register
	printf( "\t\t03 - bad first register...\n" );
	modbusBuildRequest03( &mstatus,0x20, 0xff, 0x08 );
	Test( );

	//request03 - bad register count
	printf( "\t\t03 - bad register count...\n" );
	modbusBuildRequest03( &mstatus, 0x20, 0x00, 0xff );
	Test( );

	//request03 - bad register count and first register
	printf( "\t\t03 - bad register count and first register...\n" );
	modbusBuildRequest03( &mstatus,0x20, 0xffff, 0xffff );
	Test( );

	//request03 - broadcast
	printf( "\t\t03 - broadcast...\n" );
	modbusBuildRequest03( &mstatus,0x00, 0x00, 0x08 );
	Test( );

	//request03 - other slave address
	printf( "\t\t03 - other address...\n" );
	modbusBuildRequest03( &mstatus,0x10, 0x00, 0x08 );
	Test( );

	//request06 - ok
	printf( "\t\t06 - correct request...\n" );
	modbusBuildRequest06( &mstatus,0x20, 0x06, 0xf6 );
	Test( );

	//request06 - bad CRC
	printf( "\t\t06 - bad CRC...\n" );
	modbusBuildRequest06( &mstatus,0x20, 0x06, 0xf6 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request06 - bad register
	printf( "\t\t06 - bad register...\n" );
	modbusBuildRequest06( &mstatus,0x20, 0xf6, 0xf6 );
	Test( );

	//request06 - broadcast
	printf( "\t\t06 - broadcast...\n" );
	modbusBuildRequest06( &mstatus, 0x00, 0x06, 0xf6 );
	Test( );

	//request06 - other slave address
	printf( "\t\t06 - other address...\n" );
	modbusBuildRequest06( &mstatus, 0x10, 0x06, 0xf6 );
	Test( );

	//request16 - ok
	printf( "\t\t16 - correct request...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x00, 0x04, TestValues );
	Test( );

	//request16 - bad CRC
	printf( "\t\t16 - bad CRC...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x00, 0x04, TestValues );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request16 - bad start reg
	printf( "\t\t16 - bad first register...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0xFF, 0x04, TestValues );
	Test( );

	//request16 - bad register range
	printf( "\t\t16 - bad register range...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x00, 0xFF, TestValues2 );
	Test( );

	//request16 - bad register range 2
	printf( "\t\t16 - bad register range 2...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x00, 0x20, TestValues2 );
	Test( );

	//request16 - confusing register range
	printf( "\t\t16 - confusing register range...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x00, 0x08, TestValues );
	Test( );

	//request16 - confusing register range 2
	printf( "\t\t16 - confusing register range 2...\n" );
	modbusBuildRequest16( &mstatus, 0x20, 0x01, 0x08, TestValues );
	Test( );

	//request16 - broadcast
	printf( "\t\t16 - broadcast...\n" );
	modbusBuildRequest16( &mstatus, 0x00, 0x00, 0x04, TestValues );
	Test( );

	//request16 - other slave address
	printf( "\t\t16 - other address...\n" );
	modbusBuildRequest16( &mstatus, 0x10, 0x00, 0x04, TestValues );
	Test( );

	//request02 - ok
	printf( "\t\t02 - correct request...\n" );
	modbusBuildRequest02( &mstatus, 0x20, 0x00, 0x10 );
	Test( );

	//request02 - bad CRC
	printf( "\t\t02 - bad CRC...\n" );
	modbusBuildRequest02( &mstatus, 0x20, 0x00, 0x10 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request02 - bad first discrete input
	printf( "\t\t02 - bad first discrete input...\n" );
	modbusBuildRequest02( &mstatus, 0x20, 0xff, 0x10 );
	Test( );

	//request02 - bad discrete input count
	printf( "\t\t02 - bad discrete input count...\n" );
	modbusBuildRequest02( &mstatus, 0x20, 0x00, 0xff );
	Test( );

	//request02 - bad register count and first discrete input
	printf( "\t\t02 - bad register count and first discrete input...\n" );
	modbusBuildRequest02( &mstatus, 0x20, 0xffff, 0xffff );
	Test( );

	//request02 - broadcast
	printf( "\t\t02 - broadcast...\n" );
	modbusBuildRequest02( &mstatus, 0x00, 0x00, 0x10 );
	Test( );

	//request02 - other slave address
	printf( "\t\t02 - other address...\n" );
	modbusBuildRequest02( &mstatus, 0x10, 0x00, 0x10 );
	Test( );

	//request01 - ok
	printf( "\t\t01 - correct request...\n" );
	modbusBuildRequest01( &mstatus, 0x20, 0x00, 0x04 );
	Test( );

	//request01 - bad CRC
	printf( "\t\t01 - bad CRC...\n" );
	modbusBuildRequest01( &mstatus, 0x20, 0x00, 0x04 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request01 - bad first register
	printf( "\t\t01 - bad first coil...\n" );
	modbusBuildRequest01( &mstatus, 0x20, 0xff, 0x04 );
	Test( );

	//request01 - bad register count
	printf( "\t\t01 - bad coil count...\n" );
	modbusBuildRequest01( &mstatus, 0x20, 0x00, 0xff );
	Test( );

	//request01 - bad register count and first register
	printf( "\t\t01 - bad coil count and first coil...\n" );
	modbusBuildRequest01( &mstatus, 0x20, 0xffff, 0xffff );
	Test( );

	//request01 - broadcast
	printf( "\t\t01 - broadcast...\n" );
	modbusBuildRequest01( &mstatus, 0x00, 0x00, 0x04 );
	Test( );

	//request01 - other slave address
	printf( "\t\t01 - other address...\n" );
	modbusBuildRequest01( &mstatus, 0x10, 0x00, 0x04 );
	Test( );

	//request05 - ok
	printf( "\t\t05 - correct request...\n" );
	modbusBuildRequest05( &mstatus, 0x20, 0x03, 0xff00 );
	Test( );

	//request05 - bad CRC
	printf( "\t\t05 - bad CRC...\n" );
	modbusBuildRequest05( &mstatus, 0x20, 0x03, 0xff00 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request05 - ok
	printf( "\t\t05 - correct request, non 0xff00 number...\n" );
	modbusBuildRequest05( &mstatus, 0x20, 0x03, 1 );
	Test( );

	//request05 - bad register
	printf( "\t\t05 - bad coil...\n" );
	modbusBuildRequest05( &mstatus, 0x20, 0xf3, 0xff00 );
	Test( );

	//request05 - broadcast
	printf( "\t\t05 - broadcast...\n" );
	modbusBuildRequest05( &mstatus, 0x00, 0x03, 0xff00 );
	Test( );

	//request05 - other slave address
	printf( "\t\t05 - other address...\n" );
	modbusBuildRequest05( &mstatus, 0x10, 0x03, 0xff00 );
	Test( );

	//request15 - ok
	printf( "\t\t15 - correct request...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x00, 0x04, TestValues3 );
	Test( );

	//request15 - bad CRC
	printf( "\t\t15 - bad CRC...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x00, 0x04, TestValues3 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request15 - bad start reg
	printf( "\t\t15 - bad first coil...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0xFF, 0x04, TestValues3 );
	Test( );

	//request15 - bad register range
	printf( "\t\t15 - bad coil range...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x00, 0xFF, TestValues3 );
	Test( );

	//request15 - bad register range 2
	printf( "\t\t15 - bad coil range 2...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x00, 0x20, TestValues3 );
	Test( );

	//request15 - confusing register range
	printf( "\t\t15 - confusing coil range...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x00, 0x40, TestValues3 );
	Test( );

	//request15 - confusing register range 2
	printf( "\t\t15 - confusing coil range 2...\n" );
	modbusBuildRequest15( &mstatus, 0x20, 0x01, 0x40, TestValues3 );
	Test( );

	//request15 - broadcast
	printf( "\t\t15 - broadcast...\n" );
	modbusBuildRequest15( &mstatus, 0x00, 0x00, 0x04, TestValues3 );
	Test( );

	//request15 - other slave address
	printf( "\t\t15 - other address...\n" );
	modbusBuildRequest15( &mstatus, 0x10, 0x00, 0x04, TestValues3 );
	Test( );

	//request04 - ok
	printf( "\t\t04 - correct request...\n" );
	modbusBuildRequest04( &mstatus, 0x20, 0x00, 0x04 );
	Test( );

	//request04 - bad CRC
	printf( "\t\t04 - bad CRC...\n" );
	modbusBuildRequest04( &mstatus, 0x20, 0x00, 0x04 );
	mstatus.request.frame[mstatus.request.length - 1]++;
	Test( );

	//request04 - bad first register
	printf( "\t\t04 - bad first register...\n" );
	modbusBuildRequest04( &mstatus, 0x20, 0x05, 0x04 );
	Test( );

	//request04 - bad register count
	printf( "\t\t04 - bad register count...\n" );
	modbusBuildRequest04( &mstatus, 0x20, 0x00, 0x05 );
	Test( );

	//request04 - bad register count and first register
	printf( "\t\t04 - bad register count and first register...\n" );
	modbusBuildRequest04( &mstatus, 0x20, 0x01, 0x05 );
	Test( );

	//request04 - broadcast
	printf( "\t\t04 - broadcast...\n" );
	modbusBuildRequest04( &mstatus, 0x00, 0x00, 0x04 );
	Test( );

	//request04 - other slave address
	printf( "\t\t04 - other address...\n" );
	modbusBuildRequest04( &mstatus, 0x10, 0x00, 0x04 );
	Test( );

	//WRITE PROTECTION TEST
	printf( "\t\t--Register write protection test--\n" );
	uint8_t mask[1] = { 0 };
	sstatus.registerMask = mask;
	sstatus.registerMaskLength = 1;

	modbusMaskWrite( mask, 1, 2, 1 );

	modbusBuildRequest06( &mstatus, 0x20, 2, 16 );
	Test( );
	modbusBuildRequest06( &mstatus, 0x20, 0, 16 );
	Test( );

	modbusBuildRequest16( &mstatus, 0x20, 0, 4, TestValues2 );
	Test( );
	modbusBuildRequest16( &mstatus, 0x20, 0, 2, TestValues2 );
	Test( );

	printf( "Bitval: %d\r\n", modbusMaskRead( mask, 1, 0 ) );
	printf( "Bitval: %d\r\n", modbusMaskRead( mask, 1, 1 ) );
	printf( "Bitval: %d\r\n", modbusMaskRead( mask, 1, 2 ) );
	printf( "Bitval: %d\r\n", modbusMaskRead( mask, 1, 3 ) );
	printf( "Bitval: %d\r\n", modbusMaskRead( mask, 1, 4 ) );

	sstatus.registerMaskLength = 0;
}


int main( )
{
	printf( "\n\t\t======LIBLIGHTMODBUS LIBRARY COVERAGE TEST LOG======\n\n\n" );
	memset( TestValues2, 0xAA, 1024 );

	//Init slave and master
	sstatus.registers = registers;
	sstatus.registerCount = 8;

	sstatus.coils = coils;
	sstatus.coilCount = 32;

	sstatus.discreteInputs = discreteInputs;
	sstatus.discreteInputCount = 16;

	sstatus.inputRegisters = inputRegisters;
	sstatus.inputRegisterCount = 4;
	sstatus.address = 32;

	modbusSlaveInit( &sstatus );
	modbusMasterInit( &mstatus );

	MainTest( );

	modbusSlaveEnd( &sstatus );
	modbusMasterEnd( &mstatus );

	//Reset terminal colors
	printf( "\t\t" );

	return 0;
}
