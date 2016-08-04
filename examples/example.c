#include <stdio.h>
#include <inttypes.h>

#include <lightmodbus/master.h>
#include <lightmodbus/slave.h>

//Configuration structures
ModbusMasterStatus mstatus;
ModbusSlaveStatus sstatus;

//Registers and coils
uint8_t coils[2] = { 0 };
uint16_t regs[32] = { 0 };

//For storing exit codes
uint8_t sec, mec;


//Dump slave information
void slavedump( )
{
	int i;
	printf( "==SLAVE DUMP==\n" );

	printf( "Registers:" );
	for ( i = 0; i < sstatus.registerCount; i++ )
		printf( " %d", sstatus.registers[i] );
	printf( "\n" );

	printf( "Coils:" );
	for ( i = 0; i < sstatus.coilCount >> 3; i++ )
		printf( " %d", sstatus.coils[i] );
	printf( "\n" );

	printf( "Request:" );
	for ( i = 0; i < sstatus.request.length; i++ )
		printf( " %d", sstatus.request.frame[i] );
	printf( "\n" );

	printf( "Response:" );
	for ( i = 0; i < sstatus.response.length; i++ )
		printf( " %d", sstatus.response.frame[i] );
	printf( "\n" );

	printf( "Finished: %d\n", sstatus.finished );
	printf( "Exit code: %d\n", sec );
}

//Dump master information
void masterdump( )
{
	int i;
	printf( "==MASTER DUMP==\n" );

	printf( "Received data:\n" );
	for ( i = 0; i < mstatus.dataLength; i++ )
	{
		printf( "\t slave: %d, addr: %d, type: %d, value: %d\n",
			mstatus.data[i].address, mstatus.data[i].reg, mstatus.data[i].dataType, mstatus.data[i].value );
	}
	printf( "\n" );

	printf( "Request:" );
	for ( i = 0; i < mstatus.request.length; i++ )
		printf( " %d", mstatus.request.frame[i] );
	printf( "\n" );

	printf( "Response:" );
	for ( i = 0; i < mstatus.response.length; i++ )
		printf( " %d", mstatus.response.frame[i] );
	printf( "\n" );

	printf( "Finished: %d\n", mstatus.finished );
	printf( "Exit code: %d\n\n", mec );
}

int main( )
{
	//Init slave (input registers and discrete inputs work just the same)
	sstatus.address = 27;
	sstatus.registers = regs;
	sstatus.registerCount = 32;
	sstatus.coils = coils;
	sstatus.coilCount = 16;
	modbusSlaveInit( &sstatus );

	//Init master
	modbusMasterInit( &mstatus );

	//Dump status
	slavedump( );
	masterdump( );

	/* WRITE VALUE */

	//Build frame to write single register
	modbusBuildRequest06( &mstatus, 27, 03, 56 );

	//Pretend frame is being sent to slave
	sstatus.request.frame = mstatus.request.frame;
	sstatus.request.length = mstatus.request.length;

	//Let slave parse frame
	sec = modbusParseRequest( &sstatus );

	//Pretend frame is being sent to master
	mstatus.response.frame = sstatus.response.frame;
	mstatus.response.length = sstatus.response.length;

	mec = modbusParseResponse( &mstatus );

	//Dump status again
	slavedump( );
	masterdump( );

	/* READ VALUE */

	//Build frame to read 4 registers
	modbusBuildRequest03( &mstatus, 27, 0, 4 );

	//Pretend frame is being sent to slave
	sstatus.request.frame = mstatus.request.frame;
	sstatus.request.length = mstatus.request.length;

	//Let slave parse frame
	sec = modbusParseRequest( &sstatus );

	//Pretend frame is being sent to master
	mstatus.response.frame = sstatus.response.frame;
	mstatus.response.length = sstatus.response.length;

	mec = modbusParseResponse( &mstatus );

	//Dump status again
	slavedump( );
	masterdump( );


	return 0;
}
