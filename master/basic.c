#include "../modlib.h"
#include "../parser.h"
#include "../exception.h"
#include "types.h"

#include "basic.h"

//Use external master configuration
extern MODBUSMasterStatus MODBUSMaster;

void MODBUSResponse06( union MODBUSParser *Parser )
{
	//Parse slave response to request 06 (read multiple holding registers)

	//Update frame length
	uint8_t FrameLength = 8;

	//Check frame CRC
	if ( MODBUSCRC16( ( *Parser ).Frame, FrameLength - 2 ) != ( *Parser ).Response06.CRC ) return;

	//Swap endianness
	( *Parser ).Response06.Register = MODBUSSwapEndian( ( *Parser ).Response06.Register );
	( *Parser ).Response06.Value = MODBUSSwapEndian( ( *Parser ).Response06.Value );

	//Set up new data table
	MODBUSMaster.Data = realloc( MODBUSMaster.Data, 1 );
	MODBUSMaster.Data[0].Address = ( *Parser ).Base.Address;
	MODBUSMaster.Data[0].DataType = Register;
	MODBUSMaster.Data[0].Register = ( *Parser ).Response06.Register;
	MODBUSMaster.Data[0].Value = ( *Parser ).Response06.Value;

	MODBUSMaster.Error = 0;

	//Set up data length - response successfully parsed
	MODBUSMaster.DataLength = 1;
}

uint8_t MODBUSParseResponseBasic( union MODBUSParser *Parser )
{
	//Parse response frame returned by slave using basic parsing functions
	//If 0 is returned everything is ok, and response is parsed
	//Otherwise function code is not supported and you should try full parser instead
	//Or maybe function code is wrong itself

	switch( ( *Parser ).Base.Function )
	{
		case 3: //Read multiple holding registers
			//MODBUSResponse03( Parser );
			return 0;
			break;

		case 6: //Write single holding register
			MODBUSResponse06( Parser );
			return 0;
			break;

		case 16: //Write multiple holding registers
			//MODBUSResponse16( Parser );
			return 0;
			break;

		default:
			return 1;
			break;
	}
}
