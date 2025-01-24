#ifndef DOLIBARR_FACADE_H
#define DOLIBARR_FACADE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "../IHM/Screen.h"
#include "../Errors/ErrorCode.h"

class DolibarrFacade
{
public:
    DolibarrFacade(Screen *screen, const char *dolApiRoot, const char *dolApiPath, const char *dolApiKey);
    ErrorCode getProductDataByRef(String productRef, JsonDocument &doc);
    ErrorCode addStockMovement(int productId, int warehouseId, int qty);

private:
    Screen *screen;
    String dolApiUrl;
    String dolApiKey;
    HTTPClient *dolibarr(String endpoint);
};

#endif
