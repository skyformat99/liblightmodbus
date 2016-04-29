#define _MODLIB_PARSER

#include <inttypes.h>

//This parser is incomplete yet! (basic version currently)
union MODBUSParser
{
    uint8_t Frame[256];

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
    } Base; //Base shared bytes, which have always same meaning

	struct __attribute__( ( __packed__ ) )
	{
		uint8_t Address;
		uint8_t Function;
		uint8_t ExceptionCode;
		uint16_t CRC;
	} Exception;

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstCoil;
        uint16_t CoilCount;
        uint16_t CRC;
    } Request01; //Read multiple coils

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint8_t BytesCount;
        uint8_t Values[32];
        uint16_t CRC;
    } Response01; //Read multiple coils - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstInput;
        uint16_t InputCount;
        uint16_t CRC;
    } Request02; //Read multiple discrete inputs

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint8_t BytesCount;
        uint8_t Values[32];
        uint16_t CRC;
    } Response02; //Read multiple discrete inputs - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstRegister;
        uint16_t RegisterCount;
        uint16_t CRC;
    } Request03; //Read multiple holding register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint8_t BytesCount;
        uint16_t Values[128];
        uint16_t CRC;
    } Response03; //Read multiple holding registers - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstRegister;
        uint16_t RegisterCount;
        uint16_t CRC;
    } Request04; //Read multiple input register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint8_t BytesCount;
        uint16_t Values[128];
        uint16_t CRC;
    } Response04; //Read multiple input registers - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t Coil;
        uint16_t Value;
        uint16_t CRC;
    } Request05; //Write single coil

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t Coil;
        uint16_t Value;
        uint16_t CRC;
    } Response05; //Write single coil - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t Register;
        uint16_t Value;
        uint16_t CRC;
    } Request06; //Write single holding register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t Register;
        uint16_t Value;
        uint16_t CRC;
    } Response06; //Write single holding register

	struct __attribute__( ( __packed__ ) )
    {
		uint8_t Address;
        uint8_t Function;
        uint16_t FirstCoil;
        uint16_t CoilCount;
		uint8_t BytesCount;
		uint8_t Values[32];
        uint16_t CRC;
    } Request15; //Write multiple coils

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstCoil;
        uint16_t CoilCount;
        uint16_t CRC;
    } Response15; //Write multiple coils - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstRegister;
        uint16_t RegisterCount;
        uint8_t BytesCount;
        uint16_t Values[128];
        uint16_t CRC;
    } Request16; //Write multiple holding registers

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t Address;
        uint8_t Function;
        uint16_t FirstRegister;
        uint16_t RegisterCount;
        uint16_t CRC;
    } Response16; //Write multiple holding registers
};
