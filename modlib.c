#include "modlib.h"

uint16_t MODBUSSwapEndian( uint16_t Data )
{
    //Change big-endian to little-endian and vice versa

    uint8_t Swap;

	//Create 2 bytes long union
    union Conversion
    {
        uint16_t Data;
        uint8_t Bytes[2];
    } Conversion;

	//Swap bytes
    Conversion.Data = Data;
    Swap = Conversion.Bytes[0];
    Conversion.Bytes[0] = Conversion.Bytes[1];
    Conversion.Bytes[1] = Swap;

    return Conversion.Data;
}

uint16_t MODBUSCRC16( uint8_t *Data, uint16_t Length )
{
	//Calculate CRC16 checksum using given data and length

	uint16_t CRC = 0xFFFF;
	uint16_t i;
	uint8_t j;

	for ( i = 0; i < Length; i++ )
	{
		CRC ^= (uint16_t) Data[i]; //XOR current data byte with CRC value

		for ( j = 8; j != 0; j-- )
		{
			//For each bit
			//Is least-significant-bit is set?
    		if ( ( CRC & 0x0001 ) != 0 )
			{
    			CRC >>= 1; //Shift to right and xor
    			CRC ^= 0xA001;
    		}
    		else
    			CRC >>= 1;
		}
	}
	return CRC;
}
