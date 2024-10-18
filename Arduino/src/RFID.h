#ifndef RFID_H
#define RFID_H

#include <M5Stack.h>
#include "Screen.h"
#include "MFRC522_I2C.h"

class RFID
{
public:
    RFID(Screen *screen, byte chipAddress = 0x28);
    String readHex();
    String readProductRef();

private:
    Screen *screen;
    MFRC522 mfrc522;
    String lastRfidScan = "";
    String nothingWasRead();
    String process(String scanned);
};

#endif
