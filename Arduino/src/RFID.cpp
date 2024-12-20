#include "RFID.h"

RFID::RFID(Screen *screen, byte chipAddress)
    : screen(screen), mfrc522(chipAddress)
{
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    mfrc522.PCD_Init();
}

String RFID::readHex()
{
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return nothingWasRead();
    }

    String scanned = "";

    for (int i = 0; i < mfrc522.uid.size; i++)
    {
        if (i > 0)
        {
            scanned += " ";
        }
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            scanned += "0";
        }
        scanned += String(mfrc522.uid.uidByte[i], HEX);
    }

    return process(scanned);
}

String RFID::readProductRef()
{
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return nothingWasRead();
    }

    // TODO: implement scan when you'll have a cube with rfid

    String scanned = "VERT";

    return process(scanned);
}

String RFID::nothingWasRead()
{
    screen->print("No RFID read");
    lastRfidScan = "";
    return "";
}

String RFID::process(String scanned)
{
    screen->print("RFID read " + scanned);
    if (scanned != lastRfidScan)
    {
        lastRfidScan = scanned;
        return scanned;
    }
    // screen->print("Skipping RFID because already read");
    return "";
}
