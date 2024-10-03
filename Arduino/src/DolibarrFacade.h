#ifndef DOLIBARR_FACADE_H
#define DOLIBARR_FACADE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>  
#include <WiFi.h>      

enum ErrorCode {
    SUCCESS = 0,
    WIFI_NOT_CONNECTED = 1,
    HTTP_REQUEST_FAILED = 2,
    JSON_PARSING_FAILED = 3
};

class DolibarrFacade {
public:
    DolibarrFacade(String dolapiurl, String dolapikey);
    ErrorCode addStockMovementByRef(String productRef, int qty);
    ErrorCode getProductDataByRef(String productRef, JsonDocument &doc);

private:
    String dolapiurl;
    String dolapikey;
    HTTPClient* dolibarr(String endpoint);
};

#endif
