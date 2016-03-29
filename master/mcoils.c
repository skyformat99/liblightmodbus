#include "../modlib.h"
#include "../parser.h"
#include "mtypes.h"

#include "mcoils.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

uint8_t MODBUSBuildRequest01( uint8_t Address, uint16_t FirstCoil, uint16_t CoilCount )
{
	//Build request01 frame, to send it so slave
	//Read multiple coils

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 1;
	( *Builder ).Request01.FirstCoil = MODBUSSwapEndian( FirstCoil );
	( *Builder ).Request01.CoilCount = MODBUSSwapEndian( CoilCount );

	//Calculate CRC
	( *Builder ).Request01.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}

uint8_t MODBUSBuildRequest05( uint8_t Address, uint16_t Coil, uint16_t Value )
{
	//Build request01 frame, to send it so slave
	//Write single coil

	//Set frame length
	uint8_t FrameLength = 8;

	//Set output frame length to 0 (in case of interrupts)
	MODBUSMaster.Request.Length = 0;

	//Allocate memory for frame builder
	union MODBUSParser *Builder = (union MODBUSParser *) malloc( FrameLength );

	//Reallocate memory for final frame
	MODBUSMaster.Request.Frame = (uint8_t *) realloc( MODBUSMaster.Request.Frame, FrameLength );

	Value = ( Value > 0 ) ? 0xFF00 : 0x0000;

	( *Builder ).Base.Address = Address;
	( *Builder ).Base.Function = 5;
	( *Builder ).Request05.Coil = MODBUSSwapEndian( Coil );
	( *Builder ).Request05.Value = MODBUSSwapEndian( Value );

	//Calculate CRC
	( *Builder ).Request01.CRC = MODBUSCRC16( ( *Builder ).Frame, FrameLength - 2 );

	//Copy frame from builder to output structure
	memcpy( MODBUSMaster.Request.Frame, ( *Builder ).Frame, FrameLength );

	//Free used memory
	free( Builder );

	MODBUSMaster.Request.Length = FrameLength;

	return 0;
}
