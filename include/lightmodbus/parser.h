#define _LIGHTMODBUS_PARSER

#include <inttypes.h>

union ModbusParser
{
    uint8_t frame[256];

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
    } base; //base shared bytes, which have always same meaning

	struct __attribute__( ( __packed__ ) )
	{
		uint8_t address;
		uint8_t function;
		uint8_t exceptionCode;
		uint16_t crc;
	} exception;

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstCoil;
        uint16_t coilCount;
        uint16_t crc;
    } request01; //Read multiple coils

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t byteCount;
        uint8_t values[32];
        uint16_t crc;
    } response01; //Read multiple coils - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstInput;
        uint16_t inputCount;
        uint16_t crc;
    } request02; //Read multiple discrete inputs

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t byteCount;
        uint8_t values[32];
        uint16_t crc;
    } response02; //Read multiple discrete inputs - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstRegister;
        uint16_t registerCount;
        uint16_t crc;
    } request03; //Read multiple holding reg

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t byteCount;
        uint16_t values[128];
        uint16_t crc;
    } response03; //Read multiple holding registers - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstRegister;
        uint16_t registerCount;
        uint16_t crc;
    } request04; //Read multiple input reg

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t byteCount;
        uint16_t values[128];
        uint16_t crc;
    } response04; //Read multiple input registers - response

	struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t coil;
        uint16_t value;
        uint16_t crc;
    } request05; //Write single coil

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t coil;
        uint16_t value;
        uint16_t crc;
    } response05; //Write single coil - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t reg;
        uint16_t value;
        uint16_t crc;
    } request06; //Write single holding reg

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t reg;
        uint16_t value;
        uint16_t crc;
    } response06; //Write single holding reg

	struct __attribute__( ( __packed__ ) )
    {
		uint8_t address;
        uint8_t function;
        uint16_t firstCoil;
        uint16_t coilCount;
		uint8_t byteCount;
		uint8_t values[32];
        uint16_t crc;
    } request15; //Write multiple coils

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstCoil;
        uint16_t coilCount;
        uint16_t crc;
    } response15; //Write multiple coils - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstRegister;
        uint16_t registerCount;
        uint8_t byteCount;
        uint16_t values[128];
        uint16_t crc;
    } request16; //Write multiple holding registers

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t firstRegister;
        uint16_t registerCount;
        uint16_t crc;
    } response16; //Write multiple holding registers
};
