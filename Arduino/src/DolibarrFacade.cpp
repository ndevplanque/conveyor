#include "DolibarrFacade.h"

DolibarrFacade::DolibarrFacade(String dolapiurl, String dolapikey)
    : dolapiurl(dolapiurl), dolapikey(dolapikey) {
}

HTTPClient* DolibarrFacade::dolibarr(String endpoint) {
    HTTPClient* http = new HTTPClient(); 
    String url = this->dolapiurl + endpoint;
    Serial.println(url);
    http->begin(url);
    http->addHeader("Content-Type", "application/json");
    http->addHeader("DOLAPIKEY", this->dolapikey);
    return http;
}

ErrorCode DolibarrFacade::addStockMovementByRef(String productRef, int qty) {
    if (WiFi.status() != WL_CONNECTED) {
       return WIFI_NOT_CONNECTED;
    }

    JsonDocument productData;
    ErrorCode code = getProductDataByRef(productRef, productData);
    if (code != SUCCESS) {
        return code;
    }

    int productId = productData[0]["id"];
    int warehouseId = productData[0]["fk_default_warehouse"];

    Serial.println("productId : " + String(productId) + " / warehouseId : " + String(warehouseId));

    HTTPClient* http = dolibarr("/stockmovements");
    int httpCode = http->POST(
        "{\"product_id\":" + String(productId) + ",\"warehouse_id\":" + String(warehouseId) + ",\"qty\":" + String(qty) + "}"
    );
    http->end();

    Serial.println("addStockMovementByRef : " + String(httpCode));

    if (httpCode < 200 || httpCode >= 300) {
        return HTTP_REQUEST_FAILED;
    }

    return SUCCESS;
}

ErrorCode DolibarrFacade::getProductDataByRef(String productRef, JsonDocument &doc) {
    if (WiFi.status() != WL_CONNECTED) {
        return WIFI_NOT_CONNECTED;
    }

    HTTPClient* http = dolibarr("/products?sqlfilters=t.ref:=:'" + productRef + "'&properties=id,fk_default_warehouse");
    int httpCode = http->GET();

    Serial.println("getProductDataByRef : " + String(httpCode));

    if (httpCode < 200 || httpCode >= 300) {
        http->end();
        return HTTP_REQUEST_FAILED;
    }

    String payload = http->getString();
    http->end();

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        return JSON_PARSING_FAILED;
    }

    return SUCCESS;
}
