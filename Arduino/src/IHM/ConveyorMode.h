#ifndef CONVEYOR_MODE_H
#define CONVEYOR_MODE_H

#include <Arduino.h>

enum ConveyorMode
{
    PRODUCTION = 0,
    FORWARD,
    BACKWARD,
    IDLE,
    BACKWARD_UNTIL_SCAN
};

// Tableau statique constant de chaînes correspondant aux valeurs de l'enum ConveyorMode
extern const char *translatedConveyorModes[];

inline String translateConveyorMode(ConveyorMode mode)
{
    if (mode >= PRODUCTION && mode <= IDLE)
    {
        return String(translatedConveyorModes[mode]);
    }
    return "UNKNOWN_MODE";
}

#endif
