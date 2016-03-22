#define _MODBUS_EXCEPTION

//Exception generator union

union MODBUSException
{
	uint8_t Frame[5];
	struct __attribute__( ( __packed__ ) )
	{
		uint8_t Address;
		uint8_t Function;
		uint8_t ExceptionCode;
		uint16_t CRC;
	} Exception;
};
