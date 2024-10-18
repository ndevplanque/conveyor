#ifndef RFID_H
#define RFID_H

#include <M5Stack.h>
#include "Logger.h"
#include "MFRC522_I2C.h"

class RFID
{
public:
    RFID(Logger *logger, byte chipAddress = 0x28);
    String readHex();

private:
    Logger *logger;
    MFRC522 mfrc522;
};

#endif
