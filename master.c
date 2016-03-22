#include "master.h"

//Master configurations
MODBUSMasterStatus MODBUSMaster;

void MODBUSParseResponse( uint8_t *Frame, uint8_t FrameLength )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Allocate memory for union and copy frame to it
	union MODBUSParser *Parser = malloc( FrameLength );
	memcpy( ( *Parser ).Frame,  Frame, FrameLength );

	if ( MODBUS_MASTER_BASIC )
		MODBUSParseResponseBasic( Parser );

	//Free used memory
	free( Parser );
}

void MODBUSMasterInit( )
{
	//Very basic init of master side
	MODBUSMaster.Request.Frame = malloc( 8 );
	MODBUSMaster.Request.Length = 0;
	MODBUSMaster.Data = malloc( sizeof( MODBUSData ) );
	MODBUSMaster.DataLength = 0;
}
