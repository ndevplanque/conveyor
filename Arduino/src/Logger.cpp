#include "Logger.h"

// Tableau statique de chaînes correspondant aux valeurs de l'enum ErrorCode
const char *Logger::errorCodeStrings[] = {
    "SUCCESS",
    "WIFI_NOT_CONNECTED",
    "HTTP_REQUEST_FAILED",
    "JSON_PARSING_FAILED"};

Logger::Logger()
{
}

String Logger::getUptime()
{
    unsigned long uptime = millis();

    unsigned long seconds = (uptime / 1000) % 60;
    unsigned long minutes = (uptime / (1000 * 60)) % 60;
    unsigned long hours = (uptime / (1000 * 60 * 60)) % 24;

    char buffer[9];
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);

    return String(buffer);
}

void Logger::print(String message)
{
    Serial.println("[" + this->getUptime() + "]: " + message);
}

void Logger::print(String message, ErrorCode error)
{
    Serial.println("[" + this->getUptime() + "]: " + errorCodeStrings[error] + " " + message);
}