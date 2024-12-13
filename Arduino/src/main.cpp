#include <Arduino.h>
#include <M5Stack.h>
#include "env.h"
#include "IHM/Screen.h"
#include "IHM/StateManager.h"
#include "Peripherals/RFID.h"
#include "Peripherals/StepperMotor.h"
#include "Peripherals/ServoMotor.h"
#include "Facades/DolibarrFacade.h"

Screen *screen;
RFID *rfid;
StepperMotor *stepper;
ServoMotor *servo;
DolibarrFacade *dolibarr;
StateManager *stateManager;

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
    dolibarr = new DolibarrFacade(screen, DOL_API_ROOT, DOL_API_PATH, DOL_API_KEY);

    screen->print("Pour commencer, connectez un WiFi.");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);
        screen->print("Connexion au WiFi " + String(WIFI_SSID) + "...");
    }
    screen->print("Connecte au WiFi !");
    screen->print("Utilisez les boutons pour choisir un mode.");
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
