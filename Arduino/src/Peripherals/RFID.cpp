#include "RFID.h"

RFID::RFID(byte chipAddress) : mfrc522(chipAddress)
{
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    mfrc522.PCD_Init();

    // Clé A par défaut (FF FF FF FF FF FF)
    byte defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = defaultKey[i];
    }
}

ErrorCode RFID::readData(char &result)
{
    result = ' ';
    
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return ErrorCode::RFID_READING_NOTHING;
    }

    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    byte block = 4;

    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)) != MFRC522::STATUS_OK)
    {
        return ErrorCode::RFID_AUTHENTICATION_FAILED;
    }

    if (mfrc522.MIFARE_Read(block, buffer, &size) != MFRC522::STATUS_OK)
    {
        return ErrorCode::RFID_READING_FAILED;
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    result = (char)buffer[0];

    return ErrorCode::SUCCESS;
}
