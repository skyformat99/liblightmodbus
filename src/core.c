#include "lightmodbus/core.h"

uint8_t modbusMaskRead( uint8_t *mask, uint16_t maskLength, uint16_t bit )
{
	//Return nth bit from uint8_t array
	//When 255 value is returned, an error occured

	if ( ( bit >> 3 ) >= maskLength ) return 255;
	return ( mask[bit >> 3] & ( 1 << ( bit % 8 ) ) ) >> ( bit % 8 );
}

uint8_t modbusMaskWrite( uint8_t *mask, uint16_t maskLength, uint16_t bit, uint8_t value )
{
	//Write nth bit in uint8_t array
	//When 255 value is returned, an error occured

	if ( ( bit >> 3 ) >= maskLength ) return 255;
	if ( value )
		mask[bit >> 3] |= ( 1 << ( bit % 8 ) );
	else
		mask[bit >> 3] &= ~( 1 << ( bit % 8 ) );
	return 0;
}

uint16_t modbusSwapEndian( uint16_t data )
{
    //Change big-endian to little-endian and vice versa

    uint8_t Swap;

	//Create 2 bytes long union
    union Conversion
    {
        uint16_t data;
        uint8_t Bytes[2];
    } Conversion;

	//Swap bytes
    Conversion.data = data;
    Swap = Conversion.Bytes[0];
    Conversion.Bytes[0] = Conversion.Bytes[1];
    Conversion.Bytes[1] = Swap;

    return Conversion.data;
}

uint16_t modbusCRC( uint8_t *data, uint16_t length )
{
	//Calculate CRC16 checksum using given data and length

	uint16_t crc = 0xFFFF;
	uint16_t i;
	uint8_t j;

	for ( i = 0; i < length; i++ )
	{
		crc ^= (uint16_t) data[i]; //XOR current data byte with crc value

		for ( j = 8; j != 0; j-- )
		{
			//For each bit
			//Is least-significant-bit is set?
    		if ( ( crc & 0x0001 ) != 0 )
			{
    			crc >>= 1; //Shift to right and xor
    			crc ^= 0xA001;
    		}
    		else
    			crc >>= 1;
		}
	}
	return crc;
}
