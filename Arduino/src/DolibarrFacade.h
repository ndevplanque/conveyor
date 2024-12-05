#ifndef DOLIBARR_FACADE_H
#define DOLIBARR_FACADE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "Screen.h"
#include "ErrorCode.h"

class DolibarrFacade
{
public:
    DolibarrFacade(Screen *screen, const char * dolApiRoot, const char * dolApiPath, const char * dolApiKey);
    ErrorCode addStockMovementByRef(String productRef, int qty);
    ErrorCode getProductDataByRef(String productRef, JsonDocument &doc);

private:
    Screen* screen;
    String dolApiUrl;
    String dolApiKey;
    HTTPClient *dolibarr(String endpoint);
};

#endif
