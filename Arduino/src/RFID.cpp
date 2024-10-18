#include "RFID.h"

// Constructeur par défaut
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
        return "";
    }

    String hexStr = "";

    for (int i = 0; i < mfrc522.uid.size; i++)
    {
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            hexStr += "0";
        }
        hexStr += String(mfrc522.uid.uidByte[i], HEX);
    }

    logger->print("RFID read " + hexStr);
    
    return hexStr;
}