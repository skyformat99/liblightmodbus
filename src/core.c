#include "../include/lightmodbus/core.h"

uint8_t modbusMaskRead( uint8_t *Mask, uint16_t MaskLength, uint16_t Bit )
{
	//Return nth bit from uint8_t array
	//When 255 value is returned, an error occured

	if ( ( Bit >> 3 ) >= MaskLength ) return 255;
	return ( Mask[Bit >> 3] & ( 1 << ( Bit % 8 ) ) ) >> ( Bit % 8 );
}

uint8_t modbusMaskWrite( uint8_t *Mask, uint16_t MaskLength, uint16_t Bit, uint8_t Value )
{
	//Write nth bit in uint8_t array
	//When 255 value is returned, an error occured

	if ( ( Bit >> 3 ) >= MaskLength ) return 255;
	if ( Value )
		Mask[Bit >> 3] |= ( 1 << ( Bit % 8 ) );
	else
		Mask[Bit >> 3] &= ~( 1 << ( Bit % 8 ) );
	return 0;
}

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
