//This parser is incomplete yet!

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
