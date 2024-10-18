#include "RFID.h"

RFID::RFID(Logger *logger, byte chipAddress)
    : logger(logger), mfrc522(chipAddress)
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
        logger->print("No RFID read");
        return "";
    }

    String hexStr = "";

    for (int i = 0; i < mfrc522.uid.size; i++)
    {
        if (i > 0)
        {
            hexStr += " ";
        }
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            hexStr += "0";
        }
        hexStr += String(mfrc522.uid.uidByte[i], HEX);
    }

    logger->print("RFID read " + hexStr);

    return hexStr;
}

String RFID::readProductRef()
{
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        logger->print("No RFID read");
        return "";
    }

    // TODO: implement this when you find a cube with rfid

    String ref = "VERT";

    logger->print("RFID read " + ref);

    return ref;
}
