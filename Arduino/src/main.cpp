#include <Arduino.h>
#include <M5Stack.h>
#include "DolibarrFacade.h"
#include "Logger.h"
#include "StepperMotor.h"
#include "ServoMotor.h"

Logger *logger;
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

    stepper = new StepperMotor();

    logger->print("Test du stepper...");
    stepper->turn(5);
    stepper->turn(0);
    logger->print("Le stepper bouge-t-il ?");

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
    ErrorCode error = dolibarr->addStockMovementByRef("VERT", 1);
    logger->print("addStockMovementByRef", error);

    stepper->turn(5);
    stepper->turn(0);

    delay(10000);
}