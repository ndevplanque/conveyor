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
char rfidResult;
bool isWiFiOk = false;
bool isDolibarrOk = false;

struct ServoOrder
{
    int stepperActivationCount;
    int warehouse;
};

unsigned long currentTime = 0;

unsigned long lastActionTime = 0;
unsigned long actionDelay = 100;

unsigned long lastStepperTime = 0;
unsigned long stepperDelay = 135;
int stepperActivations = 0;
ServoOrder servoOrdersAsync[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
int nextOrderKey = 0;

unsigned long lastWifiTry = 0;
unsigned long wifiTryDelay = 3000;

void beginWiFi()
{
    screen->print("Connecting to WiFi " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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

    screen->print("Press buttons to change mode.");
}

void loop()
{
    mode = stateManager->readButtons();

    currentTime = millis();

    // Manage smooth async stepper and servo movement
    if (currentTime - lastStepperTime >= stepperDelay)
    {
        lastStepperTime = currentTime;
        stepperActivations++;

        for (int i = 0; i < 4; i++)
        {
            if (stepperActivations == servoOrdersAsync[i].stepperActivationCount)
            {
                servo->goToWarehouse(servoOrdersAsync[i].warehouse);
            }
        }

        if (mode == BACKWARD || mode == BACKWARD_UNTIL_SCAN)
        {
            stepper->move(false);
        }
        if (mode == FORWARD || (mode == PRODUCTION && isDolibarrOk))
        {
            stepper->move(true);
        }
    }

    // Permanently make sure that WiFi is connected and Dolibarr is reachable
    if (currentTime - lastWifiTry >= wifiTryDelay)
    {
        lastWifiTry = currentTime;

        if (WiFi.status() != WL_CONNECTED && isWiFiOk)
        {
            screen->error("Connection lost !");
            isWiFiOk = false;
            isDolibarrOk = false;
        }
        if (WiFi.status() != WL_CONNECTED)
        {
            beginWiFi();
            return;
        }
        else if (WiFi.status() == WL_CONNECTED && !isWiFiOk)
        {
            screen->print("Connected to WiFi !");
            isWiFiOk = true;
        }

        if (!isDolibarrOk)
        {
            isDolibarrOk = dolibarr->isOnline();
            if (isDolibarrOk)
            {
                screen->print("Connected to Dolibarr API.");
            }
        }
    }

    // Handle production mode
    if (mode == PRODUCTION && currentTime - lastActionTime >= actionDelay)
    {
        lastActionTime = currentTime;

        error = rfid->readData(rfidResult);

        if (error == RFID_READING_NOTHING)
        {
            return;
        }

        msg = error == SUCCESS ? "Read " : "Scan failed ";

        screen->debug(msg + String(rfidResult), error);

        if (error != SUCCESS)
        {
            screen->error("Unknown product, human intervention required");
            stateManager->setConveyorMode(IDLE);
            return;
        }

        if (!dolibarr->isValidWarehouse(rfidResult))
        {
            screen->error("Invalid target warehouse " + String(rfidResult));
            stateManager->setConveyorMode(BACKWARD_UNTIL_SCAN);
            return;
        }

        int id = dolibarr->findIdByWarehouse(rfidResult);
        screen->print("Product " + String(id) + ", warehouse " + String(id));

        error = dolibarr->addStockMovement(id, id, 1);
        if (error != SUCCESS)
        {
            screen->error("Failed to update stock", error);
            stateManager->setConveyorMode(BACKWARD_UNTIL_SCAN);
            return;
        }

        nextOrderKey = nextOrderKey == 3 ? 0 : nextOrderKey + 1;
        servoOrdersAsync[nextOrderKey] = {stepperActivations + 20, id};
    }

    // Handle backward until scan mode
    if (mode == BACKWARD_UNTIL_SCAN && currentTime - lastActionTime >= actionDelay)
    {
        lastActionTime = currentTime;

        if (rfid->readData(rfidResult) == RFID_READING_NOTHING)
        {
            return;
        }

        screen->error("Stopping conveyor, human intervention required");

        stateManager->setConveyorMode(IDLE);
    }
}
