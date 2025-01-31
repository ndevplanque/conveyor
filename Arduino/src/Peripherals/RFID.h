#ifndef RFID_H
#define RFID_H

#include <M5Stack.h>
#include "MFRC522_I2C.h"
#include "../IHM/Screen.h"
#include "../Errors/ErrorCode.h"

class RFID
{
public:
    RFID(byte chipAddress = 0x28);
    ErrorCode readData(char &result);

private:
    MFRC522 mfrc522;
    MFRC522::MIFARE_Key key;
};

#endif
