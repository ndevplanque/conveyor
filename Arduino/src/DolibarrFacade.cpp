#include "DolibarrFacade.h"

DolibarrFacade::DolibarrFacade(String dolapiurl, String dolapikey, Logger *logs)
    : dolapiurl(dolapiurl), dolapikey(dolapikey), logs(logs)
{
}

HTTPClient *DolibarrFacade::dolibarr(String endpoint)
{
    HTTPClient *http = new HTTPClient();

    String url = this->dolapiurl + endpoint;
    http->begin(url);

    http->setTimeout(30000);
    http->addHeader("Content-Type", "application/json");
    http->addHeader("DOLAPIKEY", this->dolapikey);

    return http;
}

ErrorCode DolibarrFacade::addStockMovementByRef(String productRef, int qty)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return WIFI_NOT_CONNECTED;
    }

    JsonDocument productData;
    ErrorCode error = getProductDataByRef(productRef, productData);
    // logs->print("getProductDataByRef", error);
    if (error != SUCCESS)
    {
        return error;
    }

    int productId = productData[0]["id"];
    int warehouseId = productData[0]["fk_default_warehouse"];
    // logs->print("productId : " + String(productId) + " / warehouseId : " + String(warehouseId));

    String endpoint = "/stockmovements";
    String payload = "{\"product_id\":" + String(productId) + ",\"warehouse_id\":" + String(warehouseId) + ",\"qty\":" + String(qty) + "}";
    HTTPClient *http = dolibarr(endpoint);
    int httpCode = http->POST(payload);
    logs->print(String(httpCode) + " POST " + endpoint);

    http->end();
    delete http;

    if (httpCode < 200 || httpCode >= 300)
    {
        return HTTP_REQUEST_FAILED;
    }

    return SUCCESS;
}

ErrorCode DolibarrFacade::getProductDataByRef(String productRef, JsonDocument &doc)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return WIFI_NOT_CONNECTED;
    }

    String endpoint = "/products?sqlfilters=t.ref:=:'" + productRef + "'&properties=id,fk_default_warehouse";
    HTTPClient *http = dolibarr(endpoint);
    int httpCode = http->GET();
    logs->print(String(httpCode) + " GET " + endpoint);

    if (httpCode < 200 || httpCode >= 300)
    {
        http->end();
        delete http;
        return HTTP_REQUEST_FAILED;
    }

    String payload = http->getString();
    http->end();
    delete http;

    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        return JSON_PARSING_FAILED;
    }

    return SUCCESS;
}
