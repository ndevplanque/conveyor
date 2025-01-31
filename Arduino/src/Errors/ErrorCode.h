#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <Arduino.h>

// Définition de l'énumération des codes d'erreurs
enum ErrorCode
{
    SUCCESS = 0,
    WIFI_NOT_CONNECTED,
    HTTP_REQUEST_FAILED,
    JSON_PARSING_FAILED,
    RFID_AUTHENTICATION_FAILED,
    RFID_READING_FAILED,
    RFID_READING_NOTHING,
};

// Tableau statique constant de chaînes correspondant aux valeurs de l'enum ErrorCode
extern const char *translatedErrorCodes[];

// Fonction pour convertir un code d'erreur en chaîne de caractères
inline String translateErrorCode(ErrorCode code)
{
    if (code >= SUCCESS && code <= RFID_READING_NOTHING)
    {
        return String(translatedErrorCodes[code]); // Retourne la chaîne correspondante
    }
    return "UNKNOWN_ERROR"; // Gère les cas où l'erreur n'est pas dans l'enum
}

#endif
