#include <Arduino.h>
#include <M5Stack.h>
#include "Screen.h"
#include "RFID.h"
#include "StepperMotor.h"
#include "ServoMotor.h"
#include "DolibarrFacade.h"
#include "StateManager.h"

Screen *screen;
RFID *rfid;
StepperMotor *stepper;
ServoMotor *servo;
DolibarrFacade *dolibarr;
StateManager *stateManager;

String dolip = "http://172.20.10.8:8080";
String dolapipath = "/dolibarr/api/index.php";
String dolapikey = "2PW5SR80mSsohf0cRXn1nR1TsmV0X44j";

const char *ssid = "Delplanque";
const char *password = "nicolebg67$$1234++";

unsigned long lastActionTime = 0;
unsigned long currentTime = 0;

void setup()
{
    M5.begin();
    M5.Power.begin();

    screen = new Screen();
    rfid = new RFID(screen);
    stepper = new StepperMotor(screen);
    servo = new ServoMotor(screen);
    stateManager = new StateManager(screen);
    dolibarr = new DolibarrFacade(screen, dolip + dolapipath, dolapikey);

    screen->print("Pour commencer, connectez un WiFi.");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);
        screen->print("Connexion au WiFi " + String(ssid) + "...");
    }
    screen->print("Connecte au WiFi !");
}

void processConveyor()
{
    ConveyorMode mode = stateManager->getConveyorMode();

    if (mode == BACKWARD)
    {
        stepper->move(-1);
    }
    if (mode == FORWARD)
    {
        stepper->move(1);
    }
    if (mode == PRODUCTION)
    {
        stepper->move(1);

        String ref = rfid->readProductRef();

        if (ref != "")
        {
            ErrorCode error = dolibarr->addStockMovementByRef(ref, 1);
            screen->print("addStockMovementByRef", error);
        }
    }
}

void loop()
{
    stateManager->readButtons();

    currentTime = millis();
    if (currentTime - lastActionTime >= 750)
    {
        lastActionTime = currentTime;
        processConveyor();
    }
}
