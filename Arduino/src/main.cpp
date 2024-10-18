#include <Arduino.h>
#include <M5Stack.h>
#include "Logger.h"
#include "RFID.h"
#include "StepperMotor.h"
#include "ServoMotor.h"
#include "DolibarrFacade.h"

Logger *logger;
RFID *rfid;
StepperMotor *stepper;
ServoMotor *servo;
DolibarrFacade *dolibarr;

String dolip = "http://172.20.10.8:8080";
String dolapipath = "/dolibarr/api/index.php";
String dolapikey = "2PW5SR80mSsohf0cRXn1nR1TsmV0X44j";

const char *ssid = "Delplanque";
const char *password = "nicolebg67$$1234++";

void setup()
{
    M5.begin();
    M5.Power.begin();

    logger = new Logger();
    rfid = new RFID(logger);
    stepper = new StepperMotor(logger);
    servo = new ServoMotor(logger);
    dolibarr = new DolibarrFacade(logger, dolip + dolapipath, dolapikey);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);
        logger->print("Connexion au Wi-Fi...");
    }
    logger->print("Connecté au Wi-Fi !");
}

void loop()
{
    stepper->move(15);
    servo->move(180);

    delay(5000);

    stepper->move(0);
    servo->move(0);

    delay(5000);

    String ref = rfid->readProductRef();
    if (ref != "")
    {
        ErrorCode error = dolibarr->addStockMovementByRef(ref, 1);
        logger->print("addStockMovementByRef", error);
    }

    delay(5000);
}