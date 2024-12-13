#ifndef CONVEYOR_MANAGER_H
#define CONVEYOR_MANAGER_H

#include <M5Stack.h>
#include <Arduino.h>
#include "ConveyorMode.h"
#include "Screen.h"

class StateManager
{
public:
    StateManager(Screen *screen);
    ConveyorMode getConveyorMode();
    void setConveyorMode(ConveyorMode);
    void readButtons();

private:
    Screen *screen;
    ConveyorMode conveyorMode;
};

#endif
