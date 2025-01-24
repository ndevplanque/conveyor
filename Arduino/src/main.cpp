#include <Arduino.h>
#include <M5Stack.h>
#include "env.h"
#include "IHM/Screen.h"
#include "IHM/StateManager.h"
#include "Peripherals/RFID.h"
#include "Peripherals/StepperMotor.h"
#include "Peripherals/ServoMotor.h"
#include "Facades/DolibarrFacade.h"

// M5Stack parts
Screen *screen;
RFID *rfid;
StepperMotor *stepper;
ServoMotor *servo;
DolibarrFacade *dolibarr;
StateManager *stateManager;
ConveyorMode mode;
ErrorCode error;
String rfidScan;
String ref;

// Dev config
const bool debugMode = false;

// Servo config
const int angleA = 25;
const int angleB = 40;
const int angleC = 55;
const int angleTrash = 70;

int getAngleForWarehouse(int warehouseId)
{
    switch (warehouseId)
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

// Stepper config
const int leap = 1;
const int speed = 1000;
unsigned long stepperActivationDelay = 135;

void processStepper()
{
    if (mode == BACKWARD)
    {
        stepper->move(-leap, speed);
    }
    if (mode == PRODUCTION)
    {
        stepper->move(leap, speed);
    }
    if (mode == FORWARD)
    {
        stepper->move(leap, speed);
    }
}

// TODO: improve WiFi logic
void tryBeginWiFi()
{
    screen->print("Connexion au WiFi " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void checkWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
    screen->print("Le mode production a besoin d'etre connecte au WiFi.");
    tryBeginWiFi();
    delay(5000);
    screen->print(WiFi.status() == WL_CONNECTED ? "Connecte au WiFi !" : "Echec de la tentative de connexion.");
}

void processProduction()
{
    ref = dolibarr->translateToRef(rfid->readHex());

    if (ref == "")
    {
        return;
    }

    if (!dolibarr->isValidProductRef(ref))
    {
        screen->print("ERROR: invalid ref " + ref);
        servo->move(getAngleForWarehouse(-1));
        return;
    }

    screen->print("OK: valid ref " + ref);

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

    tryBeginWiFi();

    screen->print("Utilisez les boutons pour choisir un mode.");
}

// Loop vars
unsigned long currentTime = 0;
unsigned long lastActionTime = 0;
unsigned long lastStepperTime = 0;

void loop()
{
    mode = stateManager->readButtons();

    currentTime = millis();

    if (currentTime - lastStepperTime >= stepperActivationDelay)
    {
        lastStepperTime = currentTime;

        processStepper();
    }

    if (currentTime - lastActionTime >= 400)
    {
        lastActionTime = currentTime;

        if (mode == PRODUCTION)
        {
            processProduction();
        }
    }
}
