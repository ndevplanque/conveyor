#ifndef DOLIBARR_FACADE_H
#define DOLIBARR_FACADE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "Logger.h"
#include "ErrorCode.h"

class DolibarrFacade
{
public:
    DolibarrFacade(String dolapiurl, String dolapikey, Logger *logs);
    ErrorCode addStockMovementByRef(String productRef, int qty);
    ErrorCode getProductDataByRef(String productRef, JsonDocument &doc);

private:
    String dolapiurl;
    String dolapikey;
    Logger *logs;
    HTTPClient *dolibarr(String endpoint);
};

#endif
