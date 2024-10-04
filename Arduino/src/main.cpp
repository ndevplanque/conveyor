#include <Arduino.h>
#include "DolibarrFacade.h"
#include "Logger.h"

Logger *logs;

DolibarrFacade *dolibarr;
String dolip = "http://172.20.10.8:8080";
String dolapipath = "/dolibarr/api/index.php";
String dolapikey = "2PW5SR80mSsohf0cRXn1nR1TsmV0X44j";

const char *ssid = "Delplanque";
const char *password = "nicolebg67$$1234++";

void setup()
{
    Serial.begin(115200);

    logs = new Logger();
    dolibarr = new DolibarrFacade(dolip + dolapipath, dolapikey, logs);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        logs->print("Connexion au Wi-Fi...");
    }
    logs->print("Connecté au Wi-Fi !");
}

void loop()
{
    ErrorCode error = dolibarr->addStockMovementByRef("VERT", 1);
    logs->print("addStockMovementByRef", error);

    delay(10000);
}