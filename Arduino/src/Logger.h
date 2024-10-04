#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "ErrorCode.h"

class Logger
{
public:
    Logger();

    void print(String message);
    void print(String message, ErrorCode error);

private:
    String getUptime();
    static const char *errorCodeStrings[];
};

#endif
