#include "modlib.h"

uint16_t MODBUSSwapEndian( uint16_t Data )
{
    //Change big-endian to little-endian and vice versa

    unsigned char Swap;

	//Create 2 bytes long union
    union Conversion
    {
        uint16_t Data;
        unsigned char Bytes[2];
    } Conversion;

	//Swap bytes
    Conversion.Data = Data;
    Swap = Conversion.Bytes[0];
    Conversion.Bytes[0] = Conversion.Bytes[1];
    Conversion.Bytes[1] = Swap;

    return Conversion.Data;
}
