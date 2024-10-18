#include "StateManager.h"

// Constructeur par défaut
StateManager::StateManager(Screen *screen)
    : screen(screen), conveyorMode(IDLE)
{
    screen->drawButtons(conveyorMode);
}

ConveyorMode StateManager::getConveyorMode()
{
    return conveyorMode;
}

void StateManager::setConveyorMode(ConveyorMode mode)
{
    conveyorMode = mode;
    screen->drawButtons(conveyorMode);
    screen->print("Activation du mode " + translateConveyorMode(conveyorMode));
}

void StateManager::readButtons()
{
    M5.BtnA.read();
    if (M5.BtnA.wasPressed())
    {
        setConveyorMode(conveyorMode == BACKWARD ? IDLE : BACKWARD);
    }

    M5.BtnB.read();
    if (M5.BtnB.wasPressed())
    {
        setConveyorMode(conveyorMode == PRODUCTION ? IDLE : PRODUCTION);
    }

    M5.BtnC.read();
    if (M5.BtnC.wasPressed())
    {
        setConveyorMode(conveyorMode == FORWARD ? IDLE : FORWARD);
    }
}
