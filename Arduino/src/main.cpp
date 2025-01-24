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
ErrorCode error;

const bool debugMode = false;

const int angleA = 25;
const int angleB = 40;
const int angleC = 55;
const int angleTrash = 0;

const int leap = 300;
const int speed = 800;

unsigned long lastActionTime = 0;
unsigned long currentTime = 0;

int getAngleForWarehouse(int warehouseId)
{
    switch (key)
    {
    case 1:
        return angleA;
    case 2:
        return angleB;
    case 3:
        return angleC;
    default:
        return angleTrash;
    }
}

void checkWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
    screen->print("Le mode production a besoin d'etre connecte au WiFi.");
    screen->print("Connexion au WiFi " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
    screen->print(WiFi.status() == WL_CONNECTED ? "Connecte au WiFi !" : "Echec de la tentative de connexion.");
}

void processConveyor()
{
    stepper->waitIdle();

    ConveyorMode mode = stateManager->getConveyorMode();

    if (mode == BACKWARD)
    {
        stepper->move(-leap, speed);
    }
    if (mode == PRODUCTION)
    {
        stepper->move(leap, speed);

        String ref = rfid->readProductRef();

        if (ref != "")
        {
            checkWiFi();

            JsonDocument productData; // Document JSON pour stocker les données du produit.
            error = dolibarr->getProductDataByRef(ref, productData);
            screen->print("getProductDataByRef", error);
            if (error != SUCCESS)
            {
                return;
            }

            int productId = productData[0]["id"];
            int warehouseId = productData[0]["fk_default_warehouse"];
            screen->print("Produit " + String(productId) + ", entrepot " + String(warehouseId));

            servo->move(getAngleForWarehouse(warehouseId));

            error = dolibarr->addStockMovement(productId, warehouseId, 1);
            screen->print("addStockMovement", error);
        }
    }
    if (mode == FORWARD)
    {
        stepper->move(leap, speed);
    }
}

void setup()
{
    M5.begin();
    M5.Power.begin();

    screen = new Screen(debugMode);
    rfid = new RFID(screen);
    stepper = new StepperMotor(screen);
    servo = new ServoMotor(screen);
    stateManager = new StateManager(screen);
    dolibarr = new DolibarrFacade(screen, DOL_API_ROOT, DOL_API_PATH, DOL_API_KEY);

    checkWiFi();

    screen->print("Utilisez les boutons pour choisir un mode.");
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