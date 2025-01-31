#include <Arduino.h>
#include <M5Stack.h>
#include "env.h"
#include "IHM/Screen.h"
#include "IHM/StateManager.h"
#include "Peripherals/RFID.h"
#include "Peripherals/StepperMotor.h"
#include "Peripherals/ServoMotor.h"
#include "Facades/DolibarrFacade.h"

// Dev config
const bool debugMode = false;

// Stepper config
const int stepperMovement = 1;
const int stepperSpeed = 1000;

// M5Stack parts
Screen *screen;
RFID *rfid;
StepperMotor *stepper;
ServoMotor *servo;
DolibarrFacade *dolibarr;
StateManager *stateManager;

// Vars
ConveyorMode mode;
ErrorCode error;
String msg;
char warehouse;

void beginWiFi()
{
    screen->print("Connexion au WiFi " + String(WIFI_SSID) + "...");
    screen->print(WiFi.begin(WIFI_SSID, WIFI_PASSWORD) == WL_CONNECTED ? "Connecte au WiFi !" : "Echec de la tentative de connexion.");
}

void setup()
{
    M5.begin();
    M5.Power.begin();

    screen = new Screen(debugMode);
    rfid = new RFID();
    stepper = new StepperMotor(stepperMovement, stepperSpeed);
    servo = new ServoMotor();
    stateManager = new StateManager(screen);
    dolibarr = new DolibarrFacade(screen, DOL_API_ROOT, DOL_API_PATH, DOL_API_KEY);

    beginWiFi();

    screen->print("Utilisez les boutons pour choisir un mode.");
}

unsigned long currentTime = 0;

unsigned long lastActionTime = 0;
unsigned long actionDelay = 400;

unsigned long lastStepperTime = 0;
unsigned long stepperDelay = 135;

unsigned long lastWifiTry = 0;
unsigned long wifiTryDelay = 5000;

void loop()
{
    mode = stateManager->readButtons();

    currentTime = millis();

    // Manage smooth stepper movement
    if (currentTime - lastStepperTime >= stepperDelay)
    {
        lastStepperTime = currentTime;

        if (mode == BACKWARD)
        {
            stepper->move(false);
        }
        if (mode == PRODUCTION)
        {
            stepper->move(true);
        }
        if (mode == FORWARD)
        {
            stepper->move(true);
        }
    }

    // Permanently make sure that WiFi is connected when using Production mode
    if (mode == PRODUCTION &&
        WiFi.status() != WL_CONNECTED &&
        currentTime - lastWifiTry >= wifiTryDelay)
    {
        lastWifiTry = currentTime;

        screen->print("Le mode production a besoin d'etre connecte au WiFi.");
        beginWiFi();
    }

    // Handle production mode
    if (currentTime - lastActionTime >= actionDelay)
    {
        lastActionTime = currentTime;

        if (WiFi.status() != WL_CONNECTED)
        {
            return;
        }

        error = rfid->readData(warehouse);
        msg = error == SUCCESS ? "Read " : "Scan failed ";

        if (error != RFID_READING_NOTHING)
        {
            screen->print(msg + String(warehouse), error);
        }
        else
        {
            screen->debug("", error);
        }

        if (error != SUCCESS)
        {
            return;
        }

        if (!dolibarr->isValidWarehouse(warehouse))
        {
            screen->print("ERROR: Invalid target warehouse " + String(warehouse));
        }

        int id = dolibarr->findIdByWarehouse(warehouse);
        screen->print("Produit " + String(id) + ", entrepot " + String(id));

        error = dolibarr->addStockMovement(id, id, 1);
        if (error != SUCCESS)
        {
            screen->print("Failed to update stock", error);
            stateManager->setConveyorMode(IDLE);
        }

        servo->goToWarehouse(id);
    }
}
